#include "client.h"

// Глобальные переменные

// Сокет для связи сервера и клиента 
int client_socket; 

// Мьютекс для синхронизации доступа к очереди
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 

// Условная переменная для очереди
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; 

// Мьютекс для синхронизации доступа к списку товаров
pthread_mutex_t products_mutex = PTHREAD_MUTEX_INITIALIZER; 

// Создаём потоки
pthread_t send_thread, receive_thread;

volatile bool server_running = true;

bool send_thread_created = false;
bool receive_thread_created = false;

// Функция для потока, который получает команды от пользователя
void get_user_input() 
{
    Request request = {0};

    while (server_running) 
    {
        // Чтение команды от пользователя
		printf("1. Вывести все доступные товары\n");
		printf("2. Купить товар\n");
		printf("0. Завершение работы\n");
        printf("Выберите операцию: ");
		
        int choice;
		
		if (scanf("%d", &choice) != 1) 
		{
			fprintf(stderr, "Ошибка ввода - попробуйте снова\n");
			continue;
		}
		
		switch (choice)
		{
			case 1: 
				if(!server_running)
				{
					printf("Соединение разорвано\n");
					break;
				}
				pthread_mutex_lock(&products_mutex);
				print_products(price_list);
				pthread_mutex_unlock(&products_mutex);
				break;
			case 0:
				server_running = false; 
				break;
			case 2:
			
				if(!server_running)
				{
					printf("Соединение разорвано\n");
					break;
				}
				
				printf("Введите название товара: ");
				if (scanf("%s", request.product.name_) != 1) 
				{
					fprintf(stderr, "Ошибка ввода - попробуйте снова\n");
					break;
				}
				
				pthread_mutex_lock(&products_mutex);
				int index = find_product_index(price_list, request.product.name_);
				pthread_mutex_unlock(&products_mutex);
				
				// если резутьтан PROD_LIST_EMPTY или PROD_NOT_FOUND, тогда этот товар уже существует
				if ( (index == PROD_LIST_EMPTY) || (index == PROD_NOT_FOUND) )
				{
					printf("\nОтсутствует товар с таким (%s) названием, попробуйте снова\n", request.product.name_);
					break;
				}
				
				strcpy(request.name_request, "buy");
				request.product = price_list->products[index];
				
				printf("Введите количество товара: ");
				if (scanf("%zu", &request.product.quantity_) != 1) 
				{
					fprintf(stderr, "Ошибка ввода - попробуйте снова\n");
					break;
				}
				
				if (request.product.quantity_ == 0 || request.product.quantity_ > price_list->products[index].quantity_)
				{
					fprintf(stderr, "\nВ наличии есть всего - %zu, а вы хотите приобрести - %zu\n", price_list->products[index].quantity_, request.product.quantity_);
					break;
				}
				
				// Получение json строки из запроса
				char *str_n = request_to_json(&request);
				// Копирование json_str в request
				strcpy(request.json_str, str_n);
				free(str_n);
				
				// Добавляем сообщение в очередь
				pthread_mutex_lock(&mutex);
				queue_rcv_push_front(message_queue, request);
				pthread_cond_signal(&cond);
				pthread_mutex_unlock(&mutex);
				
				//server_running = false; // Устанавливаем флаг завершения
                break;
				
			default:
				printf("Неверный ввод. Попробуйте заново\n");
				break;
		}
		printf("\n");
    }
	
    return;
}

// Функция для потока, который отправляет сообщения на сервер
void* send_to_server(void* arg) 
{
    while (server_running) 
	{
        pthread_mutex_lock(&mutex);

        // Ожидаем, пока в очереди не появится сообщение
        while (message_queue->size == 0 && server_running) 
		{
            pthread_cond_wait(&cond, &mutex);
        }
		
		if (!server_running) 
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
		
        // Берем сообщение из очереди
        Request request = queue_rcv_pop_back(message_queue);
        pthread_mutex_unlock(&mutex);
		
		
		// Получаем JSON-строку из запроса
		char *json_str = request_to_json(&request);
		size_t data_size = strlen(json_str);

		// Конвертируем размер данных в сетевой порядок байт
		uint32_t data_size_net = htonl((uint32_t)data_size);

		// Отправляем размер данных
		ssize_t result = send(client_socket, &data_size_net, sizeof(data_size_net), 0);
		
		if (result == -1) 
		{
			fprintf(stderr, "Ошибка отправки размера данных\n");
			free(json_str);
			continue;
		}

		// Отправляем сами данные
		result = send(client_socket, json_str, data_size, 0);
		
		if (result == -1) 
		{
			fprintf(stderr, "Ошибка отправки данных\n");
			free(json_str);
			continue;
		}

		free(json_str);
		
		if (strcmp(request.name_request, "exit_client") == 0) 
		{
			server_running = false; 
			break;
		}
    }
	
	pthread_exit(NULL); 
}

// Функция для потока, который получает сообщения от сервера
void* receive_from_server(void* arg) 
{
    while (server_running) 
	{
        uint32_t data_size_net;
        ssize_t bytes_received = recv(client_socket, &data_size_net, sizeof(data_size_net), MSG_WAITALL);
        
        if (bytes_received <= 0) 
        {
            printf("\nСоединение разорвано\n");
            close(client_socket);
			
			// Устанавливаем флаг завершения
			server_running = false; 
			
			pthread_cond_signal(&cond); // Разбудить поток отправки, если он ждет
			
			// Завершаем поток
            pthread_exit(NULL);
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
				
				// Устанавливаем флаг завершения
				server_running = false;
				
				pthread_cond_signal(&cond); // Разбудить поток отправки, если он ждет
				
				// Завершаем поток
				pthread_exit(NULL);
            }
            total_received += rc;
        }
        buffer[data_size] = '\0'; // Теперь безопасно
        
        // Парсинг запроса
        Request request = {0};
        parse_json_to_request(buffer, &request);

        free(buffer);

        
		
		// Сравнение названия запроса
        if (strcmp(request.name_request, "sync_client") == 0) 
		{
			if (strcmp(request.product.name_, "ok") == 0)
			{
				// Обработка данных
				printf("\nСинхронизация окончена\n");
				continue;
			}
            pthread_mutex_lock(&products_mutex);
			add_product(price_list, request.product.name_, request.product.cost_, request.product.quantity_, request.product.description_);
			pthread_mutex_unlock(&products_mutex);
        } 
		else if (strcmp(request.name_request, "buy") == 0) 
		{
            pthread_mutex_lock(&products_mutex);
			int index = find_product_index(price_list, request.product.name_);
			
			if (request.product.cost_ == 0)
			{
				pthread_mutex_unlock(&products_mutex);
				printf("Ваш запрос на покупку не принят\n");
				printf("%s\n", request.product.description_);
				
				continue;
			}
			
			int quantity = price_list->products[index].quantity_ - request.product.quantity_;
			
			if ( quantity < 0 ) quantity = 0;
			
			index = change_product(price_list, price_list->products[index].name_, price_list->products[index].cost_, quantity, price_list->products[index].description_);
			pthread_mutex_unlock(&products_mutex);
        } 
		else if (strcmp(request.name_request, "add") == 0) 
		{
			pthread_mutex_lock(&products_mutex);
			int res = add_product(price_list, request.product.name_, request.product.cost_, request.product.quantity_, request.product.description_);
			pthread_mutex_unlock(&products_mutex);
        } 
		else if (strcmp(request.name_request, "change") == 0) 
		{
			pthread_mutex_lock(&products_mutex);
			int res = change_product(price_list, request.product.name_, request.product.cost_, request.product.quantity_, request.product.description_);
			pthread_mutex_unlock(&products_mutex);
        } 
		else if (strcmp(request.name_request, "delete") == 0) 
		{
            pthread_mutex_lock(&products_mutex);
			del_product(price_list, request.product.name_);
			pthread_mutex_unlock(&products_mutex);
        }
		else if (strcmp(request.name_request, "exit_client") == 0) 
		{
            printf("Завершение работы...\n");
			
			// Устанавливаем флаг завершения
			server_running = false; 
			
			pthread_cond_signal(&cond); // Разбудить поток отправки, если он ждет
			
			// Завершаем поток
            pthread_exit(NULL);
        } 
		else 
		{
            printf("Неизвестный запрос: %s\n", request.name_request);
        }
    }
	
	// Завершаем поток
	pthread_exit(NULL);
}

// Функция для запуска клиента
int start_client() 
{
    struct sockaddr_in server_addr;

    // Создаём сокет
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
	
    if (client_socket < 0) 
	{
		server_running = false;
        perror("Ошибка создания сокета");
        return EXIT_FAILURE;
    }

    // Настраиваем адрес сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
	
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) 
	{
		server_running = false;
        perror("Ошибка преобразования IP-адреса");
        close(client_socket);
        return EXIT_FAILURE;
    }

    // Подключаемся к серверу
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) 
	{
		server_running = false;
        perror("Ошибка подключения к серверу");
        close(client_socket);
        return EXIT_FAILURE;
    }

    printf("Подключено к серверу\n");

    if (pthread_create(&send_thread, NULL, send_to_server, NULL) != 0 ||
        pthread_create(&receive_thread, NULL, receive_from_server, NULL) != 0) 
	{
		server_running = false;
        perror("Ошибка создания потоков");
        close(client_socket);
		return EXIT_FAILURE;
    }
	
	send_thread_created = true;
	receive_thread_created = true;
	
	// отправка запроса на синхронизацию данных серверу 
	put_sync_request();
	
	return client_socket;
    //close(client_socket);
}

// отправка запроса на синхронизацию данных серверу 
void put_sync_request()
{
	// отправка запроса на синхронизацию данных серверу 
	Request request;
	memset(&request, 0, sizeof(Request));
	
	// формирование запроса
	strcpy(request.name_request, "sync_client");
	strcpy(request.product.name_, "no");
	request.product.cost_= 0;
	request.product.quantity_= 0;
	strcpy(request.product.description_, "no");
	
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
}

void clean_up()
{
	server_running = false;
	
	// Разбудить все потоки, ожидающие условной переменной
    pthread_cond_broadcast(&cond);
	
	if (send_thread_created) 
	{
        pthread_cancel(send_thread);
        pthread_join(send_thread, NULL); // Ожидаем завершения потока
    }
    if (receive_thread_created) 
	{
        pthread_cancel(receive_thread);
        pthread_join(receive_thread, NULL); // Ожидаем завершения потока
    }
	
	// Уничтожение мьютексов и условных переменных
	pthread_mutex_destroy(&mutex);
	pthread_mutex_destroy(&products_mutex);
	pthread_cond_destroy(&cond);
}