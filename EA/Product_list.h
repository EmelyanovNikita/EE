#ifndef PRODUCT_LIST_H
#define PRODUCT_LIST_H


#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h> 
#include <stddef.h>
#include "Product.h"
#include "Flags.h"

#define PROD_LIST_ERROR -1 // общая ошибка в работе с листом товаров
#define PROD_NOT_FOUND -2 // продукт не найден
#define PROD_NOT_ADDED -3 // продукт не добавлен
#define PROD_NOT_CHANGED -4 // продукт не изменён
#define PROD_LIST_EMPTY -5 // лист продуктов пуст
#define PROD_ALREADY_EXISTS -6 // продукт уже существует

typedef struct 
{ 
	// capacity - текущий размер хеш-таблицы, size - количество занятых элементов в хеш-таблице, 
	// max_size - максимальное количество заполненных элементов, чтобы коэффициент заполненности был < 0.75
	size_t size, max_size, capacity;
	
	// уазатель на массив флагов ячеек - свободна, занята, удалена
	Flags * flags_array; 
	
	// уазатель на массив товаров
	Product * products;
	
} Product_list;

// функции для работы с листом товаров 

// инициализация листа товаров
Product_list * product_list_init();

// первая хэш-функция djb2
size_t hash_funk_djb2(const char *str);

// вторая хэш-функция sdbm
size_t hash_funk_sdbm(const char *str);

// пустой ли лист товаров
bool list_is_empty(const Product_list * price_list);

// поиск ячейки по по ключу (названию), проверка существует ли товар 
int find_product_index(const Product_list * price_list, const char *str);

// поиск индекса клиента по номеру того каким он был найден ( первый найденный, второй, третий и тд )
Product* get_product_by_id(const Product_list * price_list, size_t target_index);

// добавление элемента по ключу (названию) в таблицу
int add_product(Product_list * price_list, const char *str, float cost, size_t quantity, const char * description);

// изменение товара по ключу (названию) ВМЕСТЕ С НАЗВАНИЕМ в таблице
int change_product_with_name(Product_list * price_list, const char *name, const char *new_name, float cost, size_t quantity, const char * description);

// изменение товара по ключу (названию) в таблице
int change_product(Product_list * price_list, const char *name, float cost, size_t quantity, const char * description);

// удаление (отчистка полей) элемента по ключу (названию) в таблице
int del_product(Product_list * price_list, const char *name);

// увеличение размера таблицы
int resize_prod_list (Product_list * price_list, size_t new_capacity);

// вспомогательная функция округленеи до ближайшей степени двойки
size_t round_to_power_of_two(size_t num);

// отчистка
void product_list_clear(Product_list * price_list);

// выводит все товары в терминал
void print_products(const Product_list *product_list); 

// выводит товар в терминал
void print_product(const Product_list *product_list, const char *name); 

// удаленияе
void product_list_del(Product_list * price_list);



#endif // PRODUCT_LIST_H