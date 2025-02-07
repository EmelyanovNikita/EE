#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include "QueueRcv.h"
#include "request_response.h"
#include "lib_json_db.h"
#include "Product_list.h"
#include "Product.h"

#define PORT 8055
#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024
#define FILENAME "db.json"

//#define DEBUG

// Структура для отслеживания состояния потоков
typedef struct 
{
    pthread_t thread_id;  // Идентификатор потока
    bool is_running;      // Флаг, указывающий, запущен ли поток
} ThreadInfo;


// Очередь сообщений
extern QueueRcv* message_queue;

// лист товаров
extern Product_list *price_list;

// Функция для обработки клиента в отдельном потоке
void* handle_client(void* arg);

// Функция для отправки сообщений из очереди
void* send_messages(void* arg);

// Функция для запуска сервера
void* start_server();

// отправляет запрос всем клентам кроме того, от кого пришёл этот запрос 
int sendall(Request request);

// отправляет запрос клиенту указанному в запросе
int send_client(Request request);

// отправка всего листа товаров одному клиенту
int send_client_sync(Request request);

// для работы пользователя
void get_user_input();

// функция отчистки ресурсов
void clean_up();

// инициализация массива потоков
void init_thread_pool();

// добавление потока в массив
void add_thread_to_pool(pthread_t thread_id);
#endif // SERVER_H