#include "QueueRcv.h"

// Функция для инициализации очереди
QueueRcv* queue_rcv_init() 
{
    QueueRcv* queue = (QueueRcv*)malloc(sizeof(QueueRcv));
    if (queue == NULL) 
	{
        return NULL; // Ошибка выделения памяти size
    }
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    return queue;
}

// Функция для добавления элемента в начало очереди
void queue_rcv_push_front(QueueRcv* queue, Request request) 
{
    if (queue == NULL) 
	{
        return;
    }

    // Создаём новый узел
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) 
	{
        return; // Ошибка выделения памяти
    }
	
	// Заполняем данные узла
    new_node->data = request;
	
    new_node->prev = NULL;
    new_node->next = queue->head;

    // Если очередь не пустая, обновляем указатель на предыдущий узел у текущего head
    if (queue->head != NULL) 
	{
        queue->head->prev = new_node;
    } 
	else 
	{
        // Если очередь пустая, новый узел становится и head, и tail
        queue->tail = new_node;
    }

    // Обновляем head
    queue->head = new_node;
    ++queue->size;
}

// Функция для извлечения элемента с конца очереди
Request queue_rcv_pop_back(QueueRcv* queue) 
{
    Request empty_request = { "", { "", 0.0f, 0, "" }, "", 0 };

    if (queue == NULL || queue->tail == NULL) 
	{
        return empty_request; // Проверка на NULL или пустую очередь
    }

    // Сохраняем данные из tail
    Node* tail_node = queue->tail;
	
	
    Request request = tail_node->data;
	
    // Обновляем tail
    queue->tail = tail_node->prev;
    if (queue->tail != NULL) 
	{
        queue->tail->next = NULL;
    } 
	else 
	{
        // Если очередь стала пустой, обнуляем head
        queue->head = NULL;
    }

    // Освобождаем память для удалённого узла
    free(tail_node);
    --queue->size;

    return request;
}

// Функция для освобождения памяти, выделенной для очереди
void queue_rcv_free(QueueRcv* queue) 
{
    if (queue == NULL) 
	{
        return; // Проверка на NULL
    }

    // Освобождаем все узлы очереди
    Node* current = queue->head;
    while (current != NULL) 
	{
        Node* next = current->next;
        free(current);                // Освобождаем память для узла
        current = next;
    }

    // Освобождаем память для самой структуры очереди
    free(queue);
}