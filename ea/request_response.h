#ifndef REQUEST_RESPONSE_H
#define REQUEST_RESPONSE_H

#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include "Product.h"
#define MAXLINE 30 /* максимальный размер вводимой строки */
#define MAXREQUEST 512 /* максимальный размер вводимой строки */

// Структура запроса
typedef struct 
{
    char name_request[MAXLINE]; // название запроса
    Product product; 			// товар
    char json_str[MAXREQUEST];	// запрос в виде json-строки
	int client_sock;		// клиент приславший запрос
}  Request;

// Структура ответа
typedef struct 
{
    char response[MAXLINE]; // "да" или "нет" возможно стоит сделать MAXLINE чтобы туда уместилась информация почему не был принят запрос, но это на будущее
    char json_str[MAXREQUEST];
} Response;


#endif // REQUEST_RESPONSE_H