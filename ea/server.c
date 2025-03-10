#include "server.h"

// Глобальные переменные

// Массив сокетов клиентов
int client_sockets[MAX_CLIENTS];

// Массив для отслеживания всех потоков
extern ThreadInfo thread_pool[MAX_CLIENTS + 2];  // +2 для основного потока и потока отправки

// Счетчик активных потоков
extern size_t thread_count;  

// Мьютекс для синхронизации доступа к массиву потоков
pthread_mutex_t thread_mutex = PTHREAD_MUTEX_INITIALIZER;  

// Мьютекс для синхронизации
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 

// Мьютекс для синхронизации очереди 
pthread_mutex_t mutex_queue = PTHREAD_MUTEX_INITIALIZER; 

// Мьютекс для синхронизации доступа к списку товаров
pthread_mutex_t products_mutex = PTHREAD_MUTEX_INITIALIZER; 

// Мьютекс для синхронизации доступа к бд файлу
pthread_mutex_t db_mutex = PTHREAD_MUTEX_INITIALIZER; 

// Мьютекс для синхронизации доступа к массиву сокетов клиентов
pthread_mutex_t mutex_client = PTHREAD_MUTEX_INITIALIZER; 

// Условная переменная для очереди
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; 

volatile bool server_running = true;

// Функция для обработки клиента в отдельном потоке
void* handle_client(void* arg) 
{
    int client_socket = *(int*)arg;
    free(arg); // Освобождаем память, выделенную для сокета

    //char buffer[BUFFER_SIZE];
    while (server_running) 
	{
        uint32_t data_size_net;
        
        // Читаем размер данных
        ssize_t bytes_received = recv(client_socket, &data_size_net, sizeof(data_size_net), MSG_WAITALL);
        if (bytes_received <= 0) 
        {
            printf("Клиент %d отключился\n", client_socket);
            close(client_socket);

            // Удаляем сокет из списка активных клиентов
            pthread_mutex_lock(&mutex_client);
			
            for (int i = 0; i < MAX_CLIENTS; i++) 
			{
                if (client_sockets[i] == client_socket) 
				{
                    client_sockets[i] = 0;
                    break;
                }
            }
			
            pthread_mutex_unlock(&mutex_client);
			break;
        }

        size_t data_size = ntohl(data_size_net);

        if (data_size > 1024*4096) 
        {
            fprintf(stderr, "Неверный размер данных: %zu\n", data_size);
            continue;
        }

        char* buffer = malloc(data_size + 1);
        if (!buffer) 
        {
            perror("Ошибка выделения памяти");
            continue;
        }

        size_t total_received = 0;
        while (total_received < data_size) 
        {
            ssize_t rc = recv(client_socket, buffer + total_received, data_size - total_received, 0);
            if (rc <= 0) 
            {
                free(buffer);
                printf("Ошибка чтения данных\n");
                close(client_socket);
				
				// Удаляем сокет из списка активных клиентов
				pthread_mutex_lock(&mutex_client);
				
				for (int i = 0; i < MAX_CLIENTS; i++) 
				{
					if (client_sockets[i] == client_socket) 
					{
						client_sockets[i] = 0;
						break;
					}
				}
				
				pthread_mutex_unlock(&mutex_client);
				break;
            }
            total_received += rc;
        }
		
        if (!server_running) 
        {
            free(buffer);
            break;
        }
		
        buffer[data_size] = '\0'; // Теперь безопасно

        // Парсинг запроса
        Request request = {0};
        parse_json_to_request(buffer, &request);
        request.client_sock = client_socket; // Присваиваем сокет клиента

        free(buffer);

        // Добавляем запрос в очередь
        pthread_mutex_lock(&mutex_queue);
        queue_rcv_push_front(message_queue, request);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex_queue);
    }
	
	// Удаляем сокет из списка активных клиентов
	pthread_mutex_lock(&mutex_client);
	
	for (int i = 0; i < MAX_CLIENTS; i++) 
	{
		if (client_sockets[i] == client_socket) 
		{
			client_sockets[i] = 0;
			break;
		}
	}
	
	pthread_mutex_unlock(&mutex_client);
    close(client_socket);
    pthread_exit(NULL);
}

// Функция для отправки сообщений из очереди
void* send_messages(void* arg) 
{
    while (server_running) 
	{
        pthread_mutex_lock(&mutex_queue);

        // Ожидаем, пока в очереди не появится сообщение
        while (message_queue->size == 0 && server_running) 
		{
            pthread_cond_wait(&cond, &mutex_queue);
        }
		
		if (!server_running) 
        {
            pthread_mutex_unlock(&mutex_queue);
            break;
        }

        // Берем сообщение из очереди
        Request request = queue_rcv_pop_back(message_queue);

        pthread_mutex_unlock(&mutex_queue);


		// Сравнение названия запроса
        if (strcmp(request.name_request, "sync_client") == 0) 
		{
            // Обработка синхронизации -- прислать клиенту весь лист товаров через запросы добавления товара в цикле 
            printf("\nОбработка синхронизации клиента: %d\n", request.client_sock);
			
			// отправляем все товары по одному клиенту, а когда все будут отправлены отправляем завершающий ответ
			// в котором будет указано в поле Product.name_ = ok
			send_client_sync(request);
			
			strcpy(request.product.name_, "ok");
			send_client(request);
        } 
		else if (strcmp(request.name_request, "buy") == 0) 
		{
			// поле цена при обратном ответе или рассылке будет иметь следующий смысл 0 - не принят запрос, -1 - принят
            // в описание можно вложить строку почему не приняли 
			pthread_mutex_lock(&products_mutex);
			int index = find_product_index(price_list, request.product.name_);
			
			if (request.product.quantity_ > price_list->products[index].quantity_)
			{
				pthread_mutex_unlock(&products_mutex);
				sprintf(request.product.description_, "В наличии есть всего - %zu, а вы хотите приобрести - %zu\n", price_list->products[index].quantity_, request.product.quantity_ );
				request.product.cost_ = 0;
				send_client(request);
				continue;
			}
			
			size_t quantity = price_list->products[index].quantity_ - request.product.quantity_;
			index = change_product(price_list, price_list->products[index].name_, price_list->products[index].cost_, quantity, price_list->products[index].description_);
			pthread_mutex_unlock(&products_mutex);
			
			pthread_mutex_lock(&db_mutex);
			save_products_to_json(price_list, FILENAME); 
			pthread_mutex_unlock(&db_mutex);
			
			// Отправляем сообщение всем клиентам, кроме отправителя
			sendall(request);
			
			request.product.cost_ = -1; // цена это индикатор просто 
			
			// отправитель получает ответ да товар изменён
			send_client(request);
        } 
		else if (strcmp(request.name_request, "add") == 0) 
		{
			pthread_mutex_lock(&products_mutex);
			int res = add_product(price_list, request.product.name_, request.product.cost_, request.product.quantity_, request.product.description_);
			pthread_mutex_unlock(&products_mutex);
			
			
			
			if ( res == PROD_NOT_ADDED || res == PROD_LIST_ERROR || res == PROD_ALREADY_EXISTS )
			{
				printf("\nЗапрос добавление товара - не выполнен\n");
				printf("Название: %s\n", request.product.name_);
				printf("Цена: %.3f\n", request.product.cost_);
				printf("Количество: %zu\n", request.product.quantity_);
				printf("Описание: %s\n", request.product.description_);
				
				printf("Причина - ");
				if ( res == PROD_NOT_ADDED ) printf("хэш-таблица сильно заполнена\n");
				else if ( res == PROD_LIST_ERROR ) printf("ошибка работы списка товаров\n");
				else if ( res == PROD_ALREADY_EXISTS ) printf("товар уже добавлен\n");
				continue;
			}
			
			// Отправляем сообщение всем клиентам, кроме отправителя
			sendall(request);
			
			pthread_mutex_lock(&db_mutex);
			save_products_to_json(price_list, FILENAME); 
			pthread_mutex_unlock(&db_mutex);
			
			printf("\nЗапрос добавление товара - выполнен\n");
			printf("Название: %s\n", request.product.name_);
			printf("Цена: %.3f\n", request.product.cost_);
			printf("Количество: %zu\n", request.product.quantity_);
			printf("Описание: %s\n", request.product.description_);
        } 
		else if (strcmp(request.name_request, "delete") == 0) 
		{
            pthread_mutex_lock(&products_mutex);
			int res = del_product(price_list, request.product.name_);
			pthread_mutex_unlock(&products_mutex);
			
			
			
			if ( res == PROD_NOT_ADDED )
			{
				printf("\nЗапрос на удаление - не выполнен\n");
				
				printf("Причина - ");
				if ( res == PROD_NOT_ADDED ) printf("товар не найден\n");
				continue;
			}
			
			// Отправляем сообщение всем клиентам, кроме отправителя
			sendall(request);
			
			pthread_mutex_lock(&db_mutex);
			save_products_to_json(price_list, FILENAME); 
			pthread_mutex_unlock(&db_mutex);
			
			printf("\nЗапрос на удаление - выполнен\n");
        } 
		else if (strcmp(request.name_request, "change") == 0) 
		{

            pthread_mutex_lock(&products_mutex);
			int res = change_product(price_list, request.product.name_, request.product.cost_, request.product.quantity_, request.product.description_);
			pthread_mutex_unlock(&products_mutex);
			
			
			
			if ( res == PROD_NOT_ADDED )
			{
				printf("\nЗапрос изменение товара - не выполнен\n");
				printf("Название: %s\n", request.product.name_);
				printf("Цена: %.3f\n", request.product.cost_);
				printf("Количество: %zu\n", request.product.quantity_);
				printf("Описание: %s\n", request.product.description_);
				
				printf("Причина - ");
				if ( res == PROD_NOT_ADDED ) printf("товар не найден\n");
				continue;
			}
			
			// Отправляем сообщение всем клиентам, кроме отправителя
			sendall(request);
			
			pthread_mutex_lock(&db_mutex);
			save_products_to_json(price_list, FILENAME); 
			pthread_mutex_unlock(&db_mutex);
			
			printf("\nЗапрос изменение товара - выполнен\n");
			printf("Название: %s\n", request.product.name_);
			printf("Цена: %.3f\n", request.product.cost_);
			printf("Количество: %zu\n", request.product.quantity_);
			printf("Описание: %s\n", request.product.description_);
        } 
		else if (strcmp(request.name_request, "exit_client") == 0) 
		{
            // Завершение соединения
			send_client(request);
			
            // Закрываем клиентский сокет
			pthread_mutex_lock(&mutex_client);
			
			for (int i = 0; i < MAX_CLIENTS; ++i) 
			{
				if (client_sockets[i] == request.client_sock) 
				{
					close(client_sockets[i]);
					client_sockets[i] = 0;
				}
			}
			
			pthread_mutex_unlock(&mutex_client);
        }
		else if (strcmp(request.name_request, "end") == 0) 
		{
            // Завершение работы сервера
			printf("Завершение работы сервера...\n");

			// Закрываем все клиентские сокеты
			pthread_mutex_lock(&mutex_client);
			
			for (int i = 0; i < MAX_CLIENTS; i++) 
			{
				if (client_sockets[i] != 0) 
				{
					// Завершение соединения
					send_client(request);
					close(client_sockets[i]);
					client_sockets[i] = 0;
				}
			}
			
			pthread_mutex_unlock(&mutex_client);

			pthread_exit(NULL);
        } 
		else 
		{
            printf("Неизвестный запрос: %s\n", request.name_request);
        }
    }
	
    pthread_exit(NULL);
}

// Функция для запуска сервера
void* start_server() 
{
    int server_socket, new_socket;
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);

    // Создаём сокет Inernet, TCP
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) 
	{
        perror("Ошибка создания сокета");
        exit(EXIT_FAILURE);
    }

    // Настраиваем адрес сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Привязываем сокет к адресу
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) 
	{
        perror("Ошибка привязки сокета");
        close(server_socket);
        
		// Завершаем все потоки и освобождаем ресурсы
		clean_up();  
        pthread_exit(NULL);
    }

    // Слушаем входящие подключения
    if (listen(server_socket, MAX_CLIENTS) < 0) 
	{
        perror("Ошибка прослушивания");
        close(server_socket);
		
        // Завершаем все потоки и освобождаем ресурсы
		clean_up();  
        pthread_exit(NULL);
    }

    printf("Сервер запущен на порту %d\n", PORT);

    // Создаём поток для отправки сообщений
    pthread_t sender_thread;
    if (pthread_create(&sender_thread, NULL, send_messages, NULL) != 0) 
	{
        perror("Ошибка создания потока-отправителя");
        close(server_socket);
		
		// Завершаем все потоки и освобождаем ресурсы
		clean_up();  
        pthread_exit(NULL);
    }
	else
	{
		// Добавляем поток в массив
		add_thread_to_pool(sender_thread);
	}

    // Основной цикл сервера
    while (server_running) 
	{
        // Принимаем новое подключение
        new_socket = accept(server_socket, (struct sockaddr*)&server_addr, &addr_len);
        if (new_socket < 0) 
		{
            perror("Ошибка принятия подключения");
            continue;
        }

        // Добавляем сокет клиента в массив
        pthread_mutex_lock(&mutex_client);
        for (int i = 0; i < MAX_CLIENTS; i++) 
		{
            if (client_sockets[i] == 0) 
			{
                client_sockets[i] = new_socket;
                break;
            }
        }
        pthread_mutex_unlock(&mutex_client);

        // Создаём поток для обработки клиента
        pthread_t thread_id;
		
        int* client_socket = malloc(sizeof(int));
        *client_socket = new_socket;

        if (pthread_create(&thread_id, NULL, handle_client, client_socket) != 0) 
		{
            perror("Ошибка создания потока");
            free(client_socket);
            close(new_socket);
        }
		else 
        {
            // Добавляем поток в массив
            add_thread_to_pool(thread_id);
        }
    }

    // Закрываем сокет сервера
    close(server_socket);
	
	// Ожидаем завершения потока отправки сообщений
    pthread_join(sender_thread, NULL);
	
	pthread_exit(NULL);
}

// отправляет запрос всем клентам кроме того, от кого пришёл этот запрос 
int sendall(Request request)
{
	// переделать json-строку и отправить её
	
	// Получение json строки из запроса
	char *str = request_to_json(&request);
	
	// Копирование json_str в request
	strcpy(request.json_str, str);
	free(str);
	
	pthread_mutex_lock(&mutex_client);
	
	// Выводим информацию о клиентах
    for (size_t i = 0; i < MAX_CLIENTS; ++i) 
	{
		if (client_sockets[i] != 0 && client_sockets[i] != request.client_sock) 
		{
			
			// Отправка данных
			size_t data_size = strlen(request.json_str); // Размер данных
			uint32_t data_size_net = htonl((uint32_t)data_size); // Конвертация в сетевой порядок

			pthread_mutex_lock(&mutex);
			// Сначала отправляем размер (4 байта)
			send(client_sockets[i], &data_size_net, sizeof(data_size_net), 0);

			// Затем отправляем сами данные
			data_size = send(client_sockets[i], request.json_str, data_size, 0);
			pthread_mutex_unlock(&mutex);
		}
    }
	
	pthread_mutex_unlock(&mutex_client);

	return EXIT_SUCCESS;
}

// отправляет запрос клиенту указанному в запросе
int send_client(Request request)
{
	// переделать json-строку и отправить её
	
	// Получение json строки из запроса
	char *str = request_to_json(&request);
	
	// Копирование json_str в request
	strcpy(request.json_str, str);
	free(str);
	
	#ifdef DEBUG
	printf("РАзмер запроса: %ld, %s\n", strlen(request.json_str), request.json_str);
	#endif
	
	
	// Отправка данных
	size_t data_size = strlen(request.json_str); // Размер данных
	uint32_t data_size_net = htonl((uint32_t)data_size); // Конвертация в сетевой порядок

	pthread_mutex_lock(&mutex);
	// Сначала отправляем размер (4 байта)
	send(request.client_sock, &data_size_net, sizeof(data_size_net), 0);

	// Затем отправляем сами данные
	data_size = send(request.client_sock, request.json_str, data_size, 0);
	pthread_mutex_unlock(&mutex);
	
	if (data_size == -1) 
	{
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}

// отправка всего листа товаров одному клиенту
int send_client_sync(Request request)
{
	pthread_mutex_lock(&products_mutex);
	size_t end = price_list -> size;
	pthread_mutex_unlock(&products_mutex);
	
	for ( size_t i = 1; i <= end; ++i)
	{
		pthread_mutex_lock(&products_mutex);
		Product *product = get_product_by_id(price_list, i);
		pthread_mutex_unlock(&products_mutex);
		
		if ( product == NULL ) continue;
		
		request.product = *product;
		
		send_client(request);
	}
}

// для работы пользователя
void get_user_input()
{
	Request request;
	memset(&request, 0, sizeof(Request));
	
	int index;
	
    while (server_running) 
    {
        // Чтение команды от пользователя
		printf("1. Вывести все доступные товары\n");
		printf("2. Вывести всех клиентов\n");
		printf("3. Изменить количество товара\n");
		printf("4. Изменить цену товара\n");
		printf("5. Изменить описание товара\n");
		printf("6. Добавить новый товар\n");
		printf("7. Удалить товар\n");
		printf("0. Завершение работы\n");
        printf("Выберите операцию: ");
		
        int choice;
		
		if (scanf("%d", &choice) != 1) 
		{
			fprintf(stderr, "Ошибка ввода - попробуйте снова\n");
			continue;
		}
		
		printf("\n");
		switch (choice)
		{
			case 1: 
				pthread_mutex_lock(&products_mutex);
				print_products(price_list);
				pthread_mutex_unlock(&products_mutex);
				break;
			case 2: 
				size_t iter = 0;
				printf("\nВсе сокеты подключенных клиентов:\n");
				
				pthread_mutex_lock(&mutex_client);
				for(size_t i = 0; i < MAX_CLIENTS; ++i)
				{
					if (client_sockets[i] == 0) continue;
					
					++iter;
					
					printf("%zu. %d\n", iter, client_sockets[i]);
				}
				pthread_mutex_unlock(&mutex_client);
				
				printf("\nКоличество всех подключённых клиентов: %zu\n", iter);
				
				
				break;
			
			case 3:
				printf("Введите название товара: ");
				if (scanf("%s", request.product.name_) != 1) 
				{
					fprintf(stderr, "Ошибка ввода - попробуйте снова\n");
					break;
				}
				
				pthread_mutex_lock(&products_mutex);
				index = find_product_index(price_list, request.product.name_);
				pthread_mutex_unlock(&products_mutex);
				
				// если резутьтан PROD_LIST_EMPTY или PROD_NOT_FOUND, тогда этот товар уже существует
				if ( (index == PROD_LIST_EMPTY) || (index == PROD_NOT_FOUND) )
				{
					printf("\nОтсутствует товар с таким (%s) названием, попробуйте снова\n", request.product.name_);
					break;
				}
				
				strcpy(request.name_request, "change");
				
				request.client_sock = -1;
				
				pthread_mutex_lock(&products_mutex);
				request.product = price_list->products[index];
				pthread_mutex_unlock(&products_mutex);
				
				printf("Введите количество товара: ");
				if (scanf("%zu", &request.product.quantity_) != 1) 
				{
					fprintf(stderr, "Ошибка ввода - попробуйте снова\n");
					break;
				}
				
				if (request.product.quantity_ < 0)
				{
					fprintf(stderr, "\nВ наличии должно быть 0 и больше товаров, а вы хотите изменить на - %zu\n", request.product.quantity_);
					break;
				}
				
				// Получение json строки из запроса
				char *str_n = request_to_json(&request);
				// Копирование json_str в request
				strcpy(request.json_str, str_n);
				free(str_n);
				
				// Добавляем сообщение в очередь
				pthread_mutex_lock(&mutex_queue);
				queue_rcv_push_front(message_queue, request);
				pthread_cond_signal(&cond);
				pthread_mutex_unlock(&mutex_queue);
				
				break;
			
			
			case 6:
				printf("Введите название товара: ");
				if (scanf("%s", request.product.name_) != 1) 
				{
					fprintf(stderr, "Ошибка ввода - попробуйте снова\n");
					break;
				}
				
				pthread_mutex_lock(&products_mutex);
				index = find_product_index(price_list, request.product.name_);
				pthread_mutex_unlock(&products_mutex);
				
				// если резутьтан PROD_LIST_EMPTY или PROD_NOT_FOUND, тогда этот товар уже существует
				if ( (index != PROD_NOT_FOUND) && (index != PROD_LIST_EMPTY))
				{
					printf("\nВ базе данных уже есть товар с таким (%s) названием, попробуйте снова\n", request.product.name_);
					break;
				}
				
				strcpy(request.name_request, "add");
				
				printf("Введите цену товара: ");
				if (scanf("%f", &request.product.cost_) != 1) 
				{
					fprintf(stderr, "Ошибка ввода - попробуйте снова\n");
					break;
				}
				
				printf("Введите количество товара: ");
				if (scanf("%zu", &request.product.quantity_) != 1) 
				{
					fprintf(stderr, "Ошибка ввода - попробуйте снова\n");
					break;
				}
				
				if (request.product.quantity_ < 0)
				{
					fprintf(stderr, "\nВ наличии должно быть 0 и больше товаров, а вы хотите устанвить цену - %zu\n", request.product.quantity_);
					break;
				}
				
				printf("Введите описание товара: ");
				if (scanf("%s", request.product.description_) != 1) 
				{
					fprintf(stderr, "Ошибка ввода - попробуйте снова\n");
					break;
				}
				
				// Получение json строки из запроса
				char *str_new = request_to_json(&request);
				// Копирование json_str в request
				strcpy(request.json_str, str_new);
				free(str_new);
				
				// Добавляем сообщение в очередь
				pthread_mutex_lock(&mutex_queue);
				queue_rcv_push_front(message_queue, request);
				pthread_cond_signal(&cond);
				pthread_mutex_unlock(&mutex_queue);
				
				break;
				
				
				
			case 7:
				printf("Введите название товара: ");
				if (scanf("%s", request.product.name_) != 1) 
				{
					fprintf(stderr, "Ошибка ввода - попробуйте снова\n");
					break;
				}
				
				pthread_mutex_lock(&products_mutex);
				index = find_product_index(price_list, request.product.name_);
				pthread_mutex_unlock(&products_mutex);
				
				// если резутьтан PROD_LIST_EMPTY или PROD_NOT_FOUND, тогда этот товар уже существует
				if ( (index == PROD_ALREADY_EXISTS) )
				{
					printf("\nВ базе данных уже есть товар с таким (%s) названием, попробуйте снова\n", request.product.name_);
					break;
				}
				
				strcpy(request.name_request, "delete");
				
				// Получение json строки из запроса
				char *str_n1 = request_to_json(&request);
				// Копирование json_str в request
				strcpy(request.json_str, str_n1);
				free(str_n1);
				
				// Добавляем сообщение в очередь
				pthread_mutex_lock(&mutex_queue);
				queue_rcv_push_front(message_queue, request);
				pthread_cond_signal(&cond);
				pthread_mutex_unlock(&mutex_queue);
				
				break;	
				
				
				
			case 0:
				strcpy(request.name_request, "end");
				strcpy(request.product.name_, "");
				request.product.cost_= 0;
				request.product.quantity_= 0;
				strcpy(request.product.description_, "");
				
				// Получение json строки из запроса
				char *str = request_to_json(&request);
				// Копирование json_str в request
				strcpy(request.json_str, str);
				free(str);
				
				// Добавляем сообщение в очередь
				pthread_mutex_lock(&mutex);
				queue_rcv_push_front(message_queue, request);
				pthread_cond_signal(&cond);
				pthread_mutex_unlock(&mutex);
				
				clean_up();  // Освобождаем ресурсы перед завершением
				return;
				
			default:
				printf("Неверный ввод. Попробуйте заново\n");
				break;
		}
		printf("\n");
    }

    return;
}

// функция освобождения ресурсов
void clean_up() 
{
    server_running = false;

    // Разбудить все потоки, ожидающие условной переменной
    pthread_cond_broadcast(&cond);

    // Завершение всех активных потоков
    pthread_mutex_lock(&thread_mutex);
    for (size_t i = 0; i < MAX_CLIENTS + 2; ++i) 
	{
        if (thread_pool[i].is_running) 
		{
            pthread_cancel(thread_pool[i].thread_id);
            pthread_join(thread_pool[i].thread_id, NULL); // Ожидаем завершения потока
            thread_pool[i].is_running = false;
            thread_count--;
        }
    }
    pthread_mutex_unlock(&thread_mutex);

    // Уничтожение мьютексов и условных переменных
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&products_mutex);
    pthread_mutex_destroy(&db_mutex);
    pthread_mutex_destroy(&mutex_client);
    pthread_mutex_destroy(&mutex_queue);
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&thread_mutex);
}

// инициализация массива потоков
void init_thread_pool() 
{
    pthread_mutex_lock(&thread_mutex);
	
    for (size_t i = 0; i < MAX_CLIENTS + 2; ++i) 
	{
        thread_pool[i].thread_id = 0;
        thread_pool[i].is_running = false;
    }
	
    thread_count = 0;
    pthread_mutex_unlock(&thread_mutex);
}

// добавление потока в массив
void add_thread_to_pool(pthread_t thread_id) 
{
    pthread_mutex_lock(&thread_mutex);
	
    for (size_t i = 0; i < MAX_CLIENTS + 2; ++i) {
        
		if (!thread_pool[i].is_running) 
		{
            thread_pool[i].thread_id = thread_id;
            thread_pool[i].is_running = true;
            thread_count++;
			
            break;
        }
    }
	
    pthread_mutex_unlock(&thread_mutex);
}