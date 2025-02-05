#include "server.h"
// Очередь сообщений
QueueRcv* message_queue;
// лист товаров
Product_list *price_list;

int main() 
{
	// Инициализируем очередь
    message_queue = queue_rcv_init();
	// инициализация листа товаров
	price_list = product_list_init();
	
	// получаем список товаров из бд
	int res = load_products_from_json(&price_list, FILENAME);
	if (res == LIB_JSON_DB_ERROR)
	{
		// Функция для освобождения памяти, выделенной для очереди
		queue_rcv_free(message_queue);
		// удаление листа товаров (освобождения памяти)
		product_list_del(price_list);
		printf("Ошибка получения данных из файла бд %s\n", FILENAME);
		return EXIT_FAILURE;
	}
	
	print_products(price_list); 
	
	// Создаём поток для принятия соединений клиентов
    pthread_t accepting_client_thread;
    if (pthread_create(&accepting_client_thread, NULL, start_server, NULL) != 0) 
	{
        perror("Ошибка создания потока для принятия соединений клиентов");
        exit(EXIT_FAILURE);
    }
	// Отсоединяем поток, чтобы он мог завершиться самостоятельно
    pthread_detach(accepting_client_thread);
	
	int s; //////////////////////////////////////////////////////
	scanf("%d\n", &s); //////////////////////////////////////////
	
	// Функция для освобождения памяти, выделенной для очереди
	queue_rcv_free(message_queue);
	// удаление листа товаров (освобождения памяти)
	product_list_del(price_list);
    return 0;
}