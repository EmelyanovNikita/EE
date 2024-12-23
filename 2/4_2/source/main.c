#include <stdio.h>
#include <stdlib.h>
#include "MyQueue.h"


void tests(MyQueue * queue)
{
	printf("Очередь создана\n");
	printf("Полная: %s\n", is_full(queue) == false ? "false" : "true" );
	printf("Пустая: %s\n", is_empty(queue) == false ? "false" : "true" );
	
	
	printf("\nДобавили первые 10 элементов\n");
	add_element(queue, 3, 11);
	add_element(queue, 1, 12);
	add_element(queue, 1, 13);
	add_element(queue, 1, 14);
	add_element(queue, 4, 12);
	add_element(queue, 2, 12);
	add_element(queue, 3, 12);
	add_element(queue, 2, 13);
	add_element(queue, 2, 14);
	add_element(queue, 0, 11);
	print_queue(queue);
	printf("Размер очереди: %d\n",  queue->size_ );
	
	
	printf("\nУдаляем первый элемент из очереди\n");
	pop_front(queue);
	print_queue(queue);
	printf("Размер очереди: %d\n",  queue->size_ );
	
	
	printf("\nУдаляем элементы из очереди с приоритетом = 2\n");
	delete_element(queue, 2, false);
	print_queue(queue);
	printf("Размер очереди: %d\n",  queue->size_ );
	
	
	printf("\nУдаляем элементы из очереди с приоритетом <= 4\n");
	delete_element(queue, 4, true);
	print_queue(queue);
	
	
	printf("\nДобавили 30 элементов\n");
	add_element(queue, 3, 11);
	add_element(queue, 1, 12);
	add_element(queue, 1, 13);
	add_element(queue, 1, 14);
	add_element(queue, 4, 12);
	add_element(queue, 2, 12);
	add_element(queue, 3, 12);
	add_element(queue, 2, 13);
	add_element(queue, 2, 14);
	add_element(queue, 0, 11);
	add_element(queue, 5, 11);
	add_element(queue, 7, 12);
	add_element(queue, 77, 13);
	add_element(queue, 250, 14);
	add_element(queue, 159, 12);
	add_element(queue, 179, 12);
	add_element(queue, 250, 12);
	add_element(queue, 28, 13);
	add_element(queue, 250, 14);
	add_element(queue, 23, 11);
	add_element(queue, 73, 12);
	add_element(queue, 17, 13);
	add_element(queue, 200, 14);
	add_element(queue, 139, 12);
	add_element(queue, 169, 12);
	add_element(queue, 251, 12);
	add_element(queue, 58, 13);
	add_element(queue, 230, 14);
	add_element(queue, 156, 11);
	add_element(queue, 156, 119);
	print_queue(queue);
	printf("Размер очереди: %d\n",  queue->size_ );
	
	
	printf("\nУ нас очередь сейчас достигла максимального своего размера, попробуем добавить ещё элементы\n");
	add_element(queue, 3, 11);
	add_element(queue, 1, 12);
	add_element(queue, 1, 13);
	add_element(queue, 1, 14);
	print_queue(queue);
	printf("Размер очереди: %d\n",  queue->size_ );
	
	
	printf("\nУдаляем элементы из очереди с приоритетом = 1\n");
	delete_element(queue, 1, false);
	print_queue(queue);
	printf("Размер очереди: %d\n",  queue->size_ );
	
	
	printf("\nУдаляем элементы из очереди с приоритетом <= 256\n");
	delete_element(queue, 256, true);
	print_queue(queue);
	printf("Размер очереди: %d\n",  queue->size_ );
}


int main ()
{
	MyQueue queue;
	init_values(&queue);
	
	
	tests(&queue);
}

