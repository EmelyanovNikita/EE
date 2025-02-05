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

volatile bool stop_send_thread = false;

// Функция для потока, который получает команды от пользователя
void get_user_input() 
{
    Request request;

    while (1) 
    {
        // Чтение команды от пользователя
		printf("1. Вывести все доступные товары\n");
		printf("2. Завершение работы\n");
		printf("3. Купить товар\n");
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
				pthread_mutex_lock(&products_mutex);
				print_products(price_list);
				pthread_mutex_unlock(&products_mutex);
				break;
			case 2:
				strcpy(request.name_request, "exit_client");
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
				return;
			case 3:
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
    while (1) 
	{
        pthread_mutex_lock(&mutex);

        // Ожидаем, пока в очереди не появится сообщение
        while (message_queue->size == 0) 
		{
            pthread_cond_wait(&cond, &mutex);
        }

		if (stop_send_thread) 
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
		
        // Берем сообщение из очереди
        Request request = queue_rcv_pop_back(message_queue);
        pthread_mutex_unlock(&mutex);
		
		// Отправляем сообщение на сервер
		ssize_t result = write(client_socket, request.json_str, strlen(request.json_str));
		if (result == -1) 
		{
			fprintf(stderr, "Ошибка отправки запроса\n");
			printf("Запрос: %s\n", request.name_request);
			printf("Товар : %s\n", request.product.name_);
			printf("Количество: %zu\n", request.product.quantity_);
			continue;
		}
		
		if (strcmp(request.name_request, "exit_client") == 0) 
		{
			//stop_send_thread = true; // Устанавливаем флаг завершения
			break;
		}
    }

    return NULL;
}

// Функция для потока, который получает сообщения от сервера
void* receive_from_server(void* arg) 
{
    char buffer[BUFFER_SIZE];

    while (1) 
	{
        // Чтение данных от сервера
        ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer));
		
        if (bytes_read <= 0) 
		{
            printf("\nСоединение с сервером разорвано\n");
            close(client_socket);
            exit(EXIT_FAILURE);
        }
		
		Request request;
		parse_json_to_request(buffer, &request);
		
		// Сравнение названия запроса
        if (strcmp(request.name_request, "sync_client") == 0) 
		{
			if (strcmp(request.product.name_, "ok") == 0)
			{
				printf("\nСинхронизация окончена\n");
				continue;
			}
            pthread_mutex_lock(&products_mutex);
			int res = add_product(price_list, request.product.name_, request.product.cost_, request.product.quantity_, request.product.description_);
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
		else if (strcmp(request.name_request, "delete") == 0) 
		{
            // Обработка удаления товара
            printf("Обработка удаления товара\n");
        } 
		else if (strcmp(request.name_request, "change") == 0) 
		{
            // Обработка изменения товара
            printf("Обработка изменения товара\n");
        } 
		else if (strcmp(request.name_request, "exit_client") == 0) 
		{
            printf("Завершение работы...\n");
			stop_send_thread = true; // Устанавливаем флаг завершения
			pthread_cond_signal(&cond); // Разбудить поток отправки, если он ждет
			return NULL;
        } 
		else 
		{
            printf("Неизвестный запрос: %s\n", request.name_request);
        }
    }

    return NULL;
}

// Функция для запуска клиента
int start_client() 
{
    struct sockaddr_in server_addr;

    // Создаём сокет
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
	
    if (client_socket < 0) 
	{
        perror("Ошибка создания сокета");
        exit(EXIT_FAILURE);
    }

    // Настраиваем адрес сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
	
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) 
	{
        perror("Ошибка преобразования IP-адреса");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Подключаемся к серверу
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) 
	{
        perror("Ошибка подключения к серверу");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Подключено к серверу\n");

    if (pthread_create(&send_thread, NULL, send_to_server, NULL) != 0 ||
        pthread_create(&receive_thread, NULL, receive_from_server, NULL) != 0) 
	{
        perror("Ошибка создания потоков");
        close(client_socket);
        exit(EXIT_FAILURE);
    }
	
	// отправка запроса на синхронизацию данных серверу 
	put_sync_request();

    // Отсоединяем потоки, чтобы они могли завершиться самостоятельно
    pthread_detach(send_thread);
    pthread_detach(receive_thread);
	
	return client_socket;
    //close(client_socket);
}

// отправка запроса на синхронизацию данных серверу 
void put_sync_request()
{
	// отправка запроса на синхронизацию данных серверу 
	Request request;
	
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