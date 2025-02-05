#ifndef CLIENT_H
#define CLIENT_H

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

#define SERVER_IP "127.0.0.1"
#define PORT 8055
#define BUFFER_SIZE 1024

// Очередь сообщений
extern QueueRcv* message_queue;

// лист товаров
extern Product_list *price_list;

// Функция для потока, который получает команды от пользователя
void get_user_input();

// Функция для потока, который отправляет сообщения на сервер
void* send_to_server(void* arg);

// Функция для потока, который получает сообщения от сервера
void* receive_from_server(void* arg);

// Функция для запуска клиента
int start_client();

// отправка запроса на синхронизацию данных серверу 
void put_sync_request();

#endif // CLIENT_H