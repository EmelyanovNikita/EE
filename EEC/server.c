#include "server.h"

// Глобальные переменные

// Массив сокетов клиентов
int client_sockets[MAX_CLIENTS];

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

// Функция для обработки клиента в отдельном потоке
void* handle_client(void* arg) 
{
    int client_socket = *(int*)arg;
    free(arg); // Освобождаем память, выделенную для сокета

    char buffer[BUFFER_SIZE];
    while (1) 
	{
        // Чтение данных от клиента
		//pthread_mutex_lock(&mutex);
        ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer));
		//pthread_mutex_unlock(&mutex);
        if (bytes_read <= 0) 
		{
            // Клиент отключился
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

            pthread_exit(NULL);
        }
		
        // Создаем новый запрос
        Request new_request;
		parse_json_to_request(buffer, &new_request);
		// присваем сокет откуда пришёл запрос
		new_request.client_sock = client_socket;

        // Добавляем запрос в очередь
        pthread_mutex_lock(&mutex_queue);
        queue_rcv_push_front(message_queue, new_request);

        // Оповещаем поток-отправитель о новом сообщении
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex_queue);
    }

    return NULL;
}

// Функция для отправки сообщений из очереди
void* send_messages(void* arg) 
{
    while (1) 
	{
        pthread_mutex_lock(&mutex_queue);

        // Ожидаем, пока в очереди не появится сообщение
        while (message_queue->size == 0) 
		{
            pthread_cond_wait(&cond, &mutex_queue);
        }

        // Берем сообщение из очереди
        Request request = queue_rcv_pop_back(message_queue);

        pthread_mutex_unlock(&mutex_queue);


		// Сравнение названия запроса
        if (strcmp(request.name_request, "sync_client") == 0) 
		{
            // Обработка синхронизации -- прислать клиенту весь лист товаров через запросы добавления товара в цикле 
            printf("Обработка синхронизации клиента: %d\n", request.client_sock);
			
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
				printf("Запрос добавление товара - не выполнен\n");
				printf("Название: %s\n", request.product.name_);
				printf("Цена: %.3f\n", request.product.cost_);
				printf("Количество: %zu\n", request.product.quantity_);
				printf("Описание: %s\n", request.product.description_);
				
				printf("Причина - ");
				if ( res == PROD_NOT_ADDED ) printf("хэш-таблица сильно заполнена\n");
				else if ( res == PROD_LIST_ERROR ) printf("ошибка работы листа товаров\n");
				else if ( res == PROD_ALREADY_EXISTS ) printf("товар уже добавлен\n");
				continue;
			}
			
			// Отправляем сообщение всем клиентам, кроме отправителя
			sendall(request);
			
			pthread_mutex_lock(&db_mutex);
			save_products_to_json(price_list, FILENAME); 
			pthread_mutex_unlock(&db_mutex);
			
			printf("Запрос добавление товара - выполнен\n");
			printf("Название: %s\n", request.product.name_);
			printf("Цена: %.3f\n", request.product.cost_);
			printf("Количество: %zu\n", request.product.quantity_);
			printf("Описание: %s\n", request.product.description_);
        } 
		else if (strcmp(request.name_request, "delete") == 0) 
		{
            // Обработка удаления товара
            printf("Обработка удаления товара.\n");
        } 
		else if (strcmp(request.name_request, "change") == 0) 
		{
            // Обработка изменения товара
            printf("Обработка изменения товара.\n");
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

			// Завершаем работу программы
			break;
        } 
		else 
		{
            printf("Неизвестный запрос: %s\n", request.name_request);
        }
		
        
    }

    return NULL;
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
        exit(EXIT_FAILURE);
    }

    // Слушаем входящие подключения
    if (listen(server_socket, MAX_CLIENTS) < 0) 
	{
        perror("Ошибка прослушивания");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен на порту %d\n", PORT);

    // Создаём поток для отправки сообщений
    pthread_t sender_thread;
    if (pthread_create(&sender_thread, NULL, send_messages, NULL) != 0) 
	{
        perror("Ошибка создания потока-отправителя");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
	// Отсоединяем поток, чтобы он мог завершиться самостоятельно
    pthread_detach(sender_thread);

    // Основной цикл сервера
    while (1) 
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

        // Отсоединяем поток, чтобы он мог завершиться самостоятельно
        pthread_detach(thread_id);
    }

    // Закрываем сокет сервера
    close(server_socket);
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
			pthread_mutex_lock(&mutex);
			ssize_t result = write(client_sockets[i], request.json_str, (strlen(request.json_str)));
			pthread_mutex_unlock(&mutex);
			printf("Было отправлено %zu байт\n", result);
			
			if (result == -1) 
			{
				continue;
			}
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
	
	
	size_t data_size = strlen(request.json_str);
	pthread_mutex_lock(&mutex);
	
	// Отправляем размер данных
	send(request.client_sock, &data_size, sizeof(data_size), 0); 
	
	// Отправляем сами данные
	ssize_t result = send(request.client_sock, request.json_str, data_size, 0);   
	pthread_mutex_unlock(&mutex);
	
	
	//ssize_t result = write(request.client_sock, request.json_str, (strlen(request.json_str)));
	
	printf("Было отправлено %zu байт\n", result);
	
	
	if (result == -1) 
	{
		//fprintf(stderr, "Ошибка отправки запроса:\n");
		//printf("Название: %s\n", request.name_request);
		//printf("Название: %s\n", request.product.name_);
		//printf("Название: %zu\n", request.product.quantity_);
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
		
		//printf("НАйденный товар: %s\n", product->name_);
	
		
		
		if ( product == NULL ) continue;
		
		request.product = *product;
		
		send_client(request);
	}
}