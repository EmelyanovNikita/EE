#ifndef ACTIVELIST_H
#define ACTIVELIST_H
// activelist - это список подключенных клиентов, хэш-таблица
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include "Flags.h"

#define ACTIVE_LIST_ERROR -1 // общая ошибка в работе с листом клиентов
#define CLIENT_NOT_FOUND -2  // клиент не найден
#define CLIENT_NOT_ADDED -3  // клиент не добавлен
#define CLIENT_NOT_CHANGED -4 // клиент не изменён
#define ACTIVE_LIST_EMPTY -5 // лист клиентов пуст
#define CLIENT_ALREADY_EXISTS -6 // клиент уже существует

typedef struct 
{ 
	// capacity - текущий размер хеш-таблицы, size - количество занятых элементов в хеш-таблице, 
	// max_size - максимальное количество заполненных элементов, чтобы коэффициент заполненности был < 0.75
	size_t size, max_size, capacity;
	
	// флаги из Product_list.h для работы с типом ячейки - свободна, занята, удалена
	// уазатель на массив флагов ячеек - свободна, занята, удалена
	Flags * flags_array; 
	
	// указатель на массив файловых дескрипторов клиентов
    int* client_fd;
	
} ActiveList;

// Прототипы функций
ActiveList* active_list_init();
size_t hash_funk_1(const char* str);
size_t hash_funk_2(const char* str);
bool active_list_is_empty(const ActiveList* active_list);
int find_client_index(const ActiveList* active_list, int client_fd);
int get_client_by_id(const ActiveList* active_list, size_t target_index); // индекс это номер найденного (первый найденный, второй, третий и тд)
int add_client(ActiveList* active_list, int client_fd);
int change_client(ActiveList* active_list, int old_fd, int new_fd);
int del_client(ActiveList* active_list, int client_fd);
int resize_active_list(ActiveList* active_list, size_t new_capacity);
size_t round_to_pow_two(size_t num);
void active_list_clear(ActiveList* active_list);
void print_clients(const ActiveList* active_list);
void print_client(const ActiveList* active_list, int client_fd);
void active_list_del(ActiveList* active_list);

#endif // ACTIVELIST_H