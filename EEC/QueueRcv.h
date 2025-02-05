#ifndef QUEUERCV_H
#define QUEUERCV_H

#include "request_response.h"

// Структура узла двусвязного списка
typedef struct Node
{
    Request data;          // Данные узла (структура Request)
    struct Node* prev;      // Указатель на предыдущий узел
    struct Node* next;      // Указатель на следующий узел
} Node;

// Структура двусвязного списка
typedef struct 
{
    Node* head;             // Указатель на начало списка
    Node* tail;             // Указатель на конец списка
    size_t size;            // Количество элементов в списке
} QueueRcv;

// Функция для инициализации очереди
QueueRcv* queue_rcv_init();

// Функция для добавления элемента в начало очереди
void queue_rcv_push_front(QueueRcv* queue, Request request);

// Функция для извлечения элемента с конца очереди
Request queue_rcv_pop_back(QueueRcv* queue);

// Функция для освобождения памяти, выделенной для очереди
void queue_rcv_free(QueueRcv* queue);

#endif // QUEUERCV_H