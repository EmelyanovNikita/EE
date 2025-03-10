#ifndef LIB_JSON_DB_H
#define LIB_JSON_DB_H

#include <string.h>
#include <stddef.h>
#include "Product_list.h"
#include "cJSON.h"
#include "Product.h"
#include "request_response.h"

#define LIB_JSON_DB_ERROR -1

// сохранение листа товаров в db
void save_products_to_json(Product_list *product_list, const char *filename);

// загрузка листа товаров из json db
int load_products_from_json(Product_list **product_list, const char *filename);

// конвертация листа товаров в json-строку
char* convert_product_list_to_json_string(Product_list *product_list);

// парсинг json-строки в лист товаров: просто создание листа товаров из json-строки
int parse_json_to_product_list(Product_list *product_list, const char *json_string);

// Парсинг JSON-строки в структуру Request
int parse_json_to_request(const char *json_str, Request *request);

// парсинг json-строку в ответ
Response parse_response(const char* json_str);

// Функция для формирования JSON-строки из запроса
char *request_to_json(const Request *request);

// Функция для парсинга ответа в JSON
char* parse_response_to_json(Response* res);


#endif // LIB_JSON_DB_H