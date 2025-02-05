#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
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

#endif // SERVER_H