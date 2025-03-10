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
	
	if (client_socket == -1) 
	{
        // Если подключение не удалось, освобождаем ресурсы и завершаем программу
		clean_up();
        queue_rcv_free(message_queue);
        product_list_del(price_list);
        return EXIT_FAILURE;
    }
	
	get_user_input();
	
	clean_up();
	
	// Функция для освобождения памяти, выделенной для очереди
	queue_rcv_free(message_queue);
	// удаление листа товаров (освобождения памяти)
	product_list_del(price_list);
	// Закрываем сокет
	close(client_socket);
	
    return 0;
}