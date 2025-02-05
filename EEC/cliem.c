#include "client.h"

// лист товаров
Product_list *price_list;
// Очередь сообщений
QueueRcv* message_queue; 

int main() 
{
	// Инициализируем очередь
    message_queue = queue_rcv_init();
	// инициализация листа товаров
	price_list = product_list_init();
	
    int client_socket = start_client();
	
	get_user_input();
	
	pthread_exit(NULL);
	
	// Функция для освобождения памяти, выделенной для очереди
	queue_rcv_free(message_queue);
	// удаление листа товаров (освобождения памяти)
	product_list_del(price_list);
	// Закрываем сокет
	close(client_socket);
	
    return 0;
}