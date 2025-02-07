#include "Product_list.h"


// инициализация листа товаров, первая применяется сразу после заведения переменной
// Пример Product_list * price_list = product_list_init();

Product_list * product_list_init()
{
	//выделяем память под лист товаров
	Product_list * price_list = malloc ( sizeof(Product_list) );
	
	//обработка исключения malloc
	if ( price_list == NULL ) 
	{
		return NULL;
	}
	
	// заполняем нулевыми значениями тк может будет использована только далеко в будущем
	price_list -> size = price_list -> max_size = price_list -> capacity = 0;
	price_list -> flags_array = NULL;
	price_list -> products = NULL;
	
	return price_list;
}

// первая хэш-функция djb2
size_t hash_funk_djb2(const char *str)
{
	size_t hash = 5381;
    int c;

    while (c = *str++)
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

// вторая хэш-функция sdbm
size_t hash_funk_sdbm(const char *str)
{
	size_t hash = 0;
    int c;

    while (c = *str++)
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}

// пустой ли лист товаров
bool list_is_empty(const Product_list * price_list)
{
	// проверям количество занятых элементов в таблице на 0
	if ( price_list -> size == 0 ) return true;
	return false;
}

// поиск ячейки по по ключу (названию), проверка существует ли товар 
int find_product_index(const Product_list * price_list, const char *str)
{
	// проверяем пустой ли лист товаров и в случае если он пустой возращаем соответвующее значение
	if( list_is_empty(price_list) ) return PROD_LIST_EMPTY;
	
	// маска для лучшего округления
	size_t mask = price_list -> capacity - 1;
	
	// расчитываем индексы через хэш-функции
	size_t x = hash_funk_djb2(str) & mask;
	size_t y = hash_funk_sdbm(str) & mask;
		
	// переменная для подсчёта итерации, и переменная для максимального количества итераций
	size_t iter = 0;
	size_t max_iter = (price_list -> capacity) * 2;
	
	// так как поиск нам надо когда нибудь закончить поиск, примем за максимальное количество 
	// итераций размер таблицы * 2, чтобы гарантированно все возможные ячейки были проверены 
	while ( (price_list -> flags_array[x] != FREE) && (iter <= max_iter) )
	{
		// условие выхода это когда у нас ячейка занята OCCUPIED и название совпадает
		if ( (price_list -> flags_array[x] == OCCUPIED) 
			&& ( strcmp(price_list -> products[x].name_, str) == 0 ) )
		{
			return x;
		}
		
		// если не свопадает имя или флаг DELETED тогда пересчитываем индекс заново 
		++iter; 
		x = (x + iter * y) & mask;	
	}
	
	return PROD_NOT_FOUND;
}

// Поиск индекса клиента по номеру того каким он был найден ( первый найденный, второй, третий и тд )
Product* get_product_by_id(const Product_list * price_list, size_t target_index)
{
	// проверка списка на пустоту
	if (list_is_empty(price_list)) return NULL;
	
	// проверка на правильность переданного индекса
	if (target_index > (price_list->size))
	{
		// возврат NULL в случае если переданный индекс больше чем size (количество занятых элементов в хеш-таблице)
		return NULL;
	}
	
	size_t index_found_client = 0;
	
	for (size_t i = 0; i < (price_list->capacity) ; ++i)
	{
		if ( price_list->flags_array[i] == OCCUPIED )
		{
			++index_found_client;
			if (index_found_client == target_index)
			{
				return &price_list->products[i];
			}
		}
	}
	
	return NULL;
}

// добавление элемента по ключу (названию) в таблицу
// принимает указатель на лист товаров, название, цену, количество, строку с описанием возвращает: 
// 1) В случае успеха: индекс ячейки в которую был вставлен товар 
// 2) В случае не успеха: PROD_LIST_ERROR (общая ошибка для листа товаров, в данном случае: не увеличилась память) 
// или PROD_NOT_ADDED (товар не был добавлен)
int add_product(Product_list * price_list, const char *name, float cost, size_t quantity, const char * description)
{
	// проверяем наличие/отсутствие товара
	int res = find_product_index(price_list, name);
	
	// если резутьтан не PROD_LIST_EMPTY или не PROD_NOT_FOUND, тогда этот товар уже существует
	if ( (res != PROD_LIST_EMPTY) && (res != PROD_NOT_FOUND) )
	{
		return PROD_ALREADY_EXISTS;
	}
	
	// если текущее количество заполенных элементов в таблице приближается к граничному значению, 
	// те коэффицент заполненности становится > 0.75, тогда делаем resize
	if ( (price_list -> size) >= (price_list -> max_size) )
	{
		// увеличиваем размер хэш-таблицы на 50% // 8 это в случае если изначально был 0
		size_t new_capacity = ( price_list -> capacity == 0 ) ? 8 : (price_list -> capacity * 15) / 10; 
		res = resize_prod_list( price_list, new_capacity );
		
		// если в resize_prod_list возникла ошибка, тогда выходим из функции
		if ( res == PROD_LIST_ERROR )
		{
			return PROD_LIST_ERROR;
		}
	}
	
	// маска для лучшего округления
	size_t mask = price_list -> capacity - 1;
	
	// расчитываем хэш-функции
	size_t x = hash_funk_djb2(name) & mask;
	size_t y = hash_funk_sdbm(name) & mask;
	
	// счититаем хэш-функцию количество раз равное размеру таблицы
	for (size_t i = 0; i < (price_list -> capacity); ++i)
	{
		// если ячейка не занята, тогда занимаем её нашим товаром
		if ( price_list -> flags_array[x] != OCCUPIED )
		{
			price_list -> flags_array[x] = OCCUPIED;
			strcpy(price_list -> products[x].name_, name);
			price_list->products[x].name_[MAXLINE - 1] = '\0';
			price_list -> products[x].cost_ = cost;
			price_list -> products[x].quantity_ = quantity;
			strcpy(price_list -> products[x].description_, description);
			price_list->products[x].description_[MAXTEXT - 1] = '\0';
			
			// увеличиваем количество занятых элементов
			++(price_list -> size);
			
			// возвращаем индекс ячейки в которую был вставлен товар
			return x;
		}
		
		// иначе пересчитываем индекс ячейки
		x = (x + i * y) & mask; 
	}
	
	return PROD_NOT_ADDED;
}

// изменение товара по ключу (названию) ВМЕСТЕ С НАЗВАНИЕМ в таблице
// принимает указатель на лист товаров, название старое товара, название новое товара, цену, количество, описание и возвращает: 
// 1) В случае успеха: индекс ячейки в которой был изменён товар 
// 2) В случае не успеха: PROD_NOT_CHANGED (товар не изменён), PROD_NOT_ADDED нет товара с старым именем 
int change_product_with_name(Product_list * price_list, const char *name, const char *new_name, float cost, size_t quantity, const char * description)
{
	// убедимся, что товар есть в таблице 
	int index = find_product_index(price_list, name);
	
	// если нет такого товара, тогда выходим с соответсвующим возвращаемым значением
	if ( (index == PROD_LIST_EMPTY) || (index == PROD_NOT_FOUND) )
	{
		return PROD_NOT_ADDED;
	}
	
	// изначально добавим товар с новым именем, не удаляя ячейку с прошлым, дабы не получилось ситуация с потерей данных 
	// (убедимся что мы можем вставить новое имя)
	index = add_product(price_list, new_name , cost, quantity, description);
	
	// если результат PROD_NOT_ADDED PROD_LIST_ERROR PROD_ALREADY_EXISTS тогда товар не изменён
	if ( (index == PROD_NOT_ADDED) || (index == PROD_LIST_ERROR) || (index == PROD_ALREADY_EXISTS) )
	{
		return PROD_NOT_CHANGED;
	}
	
	// теперь удаляем товар 
	int last_index = del_product(price_list, name);
	
	// если результат PROD_NOT_ADDED товара с таким именем нет в таблице
	if ( last_index == PROD_NOT_ADDED )
	{
		return PROD_NOT_ADDED;
	}
	
	// возвращаем индекс товара
	return index;
}

// изменение товара по ключу (названию) в таблице
// принимает указатель на лист товаров, название старое товара, цену, количество, описание и возвращает: 
// 1) В случае успеха: индекс ячейки из которой был изменён товар 
// 2) В случае не успеха: PROD_NOT_ADDED (товар не существует в таблице)
int change_product(Product_list * price_list, const char *name, float cost, size_t quantity, const char * description)
{
	// проверяем наличие/отсутствие товара
	int index = find_product_index(price_list, name);
	
	// если результат не PROD_LIST_EMPTY или не PROD_NOT_FOUND, тогда этот товар не существует
	if ( (index == PROD_LIST_EMPTY) || (index == PROD_NOT_FOUND) )
	{
		return PROD_NOT_ADDED;
	}
	
	// заполнение полей данного элемента новыми значениями, всё кроме названия
	price_list -> products[index].cost_ = cost;
	price_list -> products[index].quantity_ = quantity;
	
	memmove(price_list->products[index].description_, description, strlen(description) + 1);
	
	return index;
}

// удаление (отчистка полей) элемента по ключу (названию) в таблице
// принимает указатель на лист товаров, название и возвращает: 
// 1) В случае успеха: индекс ячейки из которой был удалён товар 
// 2) В случае не успеха: PROD_NOT_ADDED (товар не существует в таблице)
int del_product(Product_list * price_list, const char *name)
{
	// изменяем все поля кроме имени на дэфолные значения
	int index = change_product(price_list, name, 0, 0, "");
	
	// если результат PROD_NOT_ADDED товара с таким именем нет в таблице
	if ( index == PROD_NOT_ADDED )
	{
		return PROD_NOT_ADDED;
	}
	
	// изменяем имя
	strcpy(price_list -> products[index].name_, "");
	
	// меняем флаг на удалён
	price_list -> flags_array[index] = DELETED;
	
	// уменьшаем количество заполненных элементов
	--(price_list -> size);
	
	// возвращаем индекс на удалённый элемент
	return index;
}

// увеличение размера таблицы
// принимает указатель на лист товара, новый размер хэш-таблицы 
int resize_prod_list (Product_list * price_list, size_t new_capacity)
{
	// проверяем, нужно ли вообще расширять хеш-таблицу
	if (price_list -> capacity < price_list -> max_size) 
	{
		// так как это не ошибка, а просто ложный выщов возвращаем просто 0
		return 0;
	}
	
	// округляем запрошенную ёмкость до ближайшей большей степени двойки
	new_capacity = round_to_power_of_two(new_capacity);
	
	// если результат меньше текущей ёмкости тогда умножаем на два
	if (new_capacity <= price_list -> capacity) 
	{ 
		new_capacity <<= 1; 
	} 
	
	// выделяем память под массив флагов
	Flags * new_flags_array = malloc( sizeof(Flags) * new_capacity ); 
	
	// проверка выделилась ли память
	if (!new_flags_array) 
	{ 
		// в случае ошибки выделения памяти возвращаем PROD_LIST_ERROR
		return  PROD_LIST_ERROR;
	}
	
	// выделяем память под массив товаров
	Product * new_products = malloc( sizeof(Product) * new_capacity );
	
	// проверка выделилась ли память
	if (!new_products) 
	{
		// в случае ошибки выделения памяти возвращаем PROD_LIST_ERROR, но уже освободив ранее выделенную память
		free(new_flags_array); 
		return  PROD_LIST_ERROR;
	}
	
	// заполняем все флаги FREE
	for (size_t i = 0; i < new_capacity; ++i) 
	{
		new_flags_array[i] = FREE;
    }
	
	// маска для лучшего округления
	size_t mask = new_capacity - 1;
	
	// переносим все элементы из первой таблицы в вторую
	for (size_t i = 0; i < price_list -> capacity; ++i) 
	{ 
		// если ячейка занята, тогда переносим, иначе переходим к следующей итерации
		if (price_list -> flags_array[i] != OCCUPIED) continue; 
		
		// расчитываем хэш-функции
		size_t x = hash_funk_djb2(price_list -> products[i].name_) & mask;
		size_t y = hash_funk_sdbm(price_list -> products[i].name_) & mask;
		
		// тк у нас таблица должна быть заполнена с коэффицентом максимум 0.75 мы предпологаем что у нас 
		// индекс свободной ячейки должен быть найден, тем более таблица большое прошлой
		
		// переменная для подсчёта итерации 
		size_t iter = 0;
		while (new_flags_array[x] != FREE) 
		{ 
			++iter;
			x = (x + iter * y) & mask; 
		} 
		
		// указываем ячейку как занятую и присваиваем все элементы из прошлой таблицы
		new_flags_array[x] = OCCUPIED; 
		strcpy(new_products[x].name_,  price_list -> products[i].name_);
		new_products[x].cost_ = price_list -> products[i].cost_;
		new_products[x].quantity_ = price_list -> products[i].quantity_;
		strcpy(new_products[x].description_,  price_list -> products[i].description_);
	}
	
	
	// освобождаем массивы 
	free(price_list -> flags_array);
    free(price_list -> products);
	
	// присваиваем новые значения всем нашим полям
	price_list -> flags_array = new_flags_array;
	price_list -> products = new_products;
	price_list -> capacity = new_capacity;
	
	// пересчитываем количество элементов, максимально допустимое для того чтобы коэффицент заполнения был 0.75
	price_list -> max_size = (new_capacity >> 1) + (new_capacity >> 2);
	return 0;
}

// вспомогательная функция округленеи до ближайшей степени двойки
size_t round_to_power_of_two(size_t num) 
{
	// 0 округляем до 1
    if (num == 0) return 1; 
	
	// уменьшаем на 1 для корректного округления числа которое уже является степенью двойки
    num--;
	
	// устанавливаем биты
    num |= num >> 1; 
    num |= num >> 2;
    num |= num >> 4;
    num |= num >> 8;
	
	// для 32-битных чисел
    num |= num >> 16; 
	
	// возвращаем следующую степень двойки
    return num + 1; 
}

// отчистка листа товаров
// пример работы product_list_clear(price_list);

void product_list_clear(Product_list * price_list)
{
	//устанавливаем счётчик заполеннных ячеек в zero
    price_list -> size = 0;
	
	// отчищаем память в массивах = заполняем память определенным символом  
	// если они уже не NULL
    if ( (price_list -> flags_array) && (price_list -> products) ) 
	{
		// отчищаем в листе товаров каждый товар по отдельности, также устанавливаем все флаги в free
		for (size_t i = 0; i < (price_list -> capacity); ++i) 
		{
            memset(price_list->products[i].name_, 0, MAXLINE);
			price_list -> flags_array[i] = FREE;
            price_list -> products[i].cost_ = 0.0f;
            price_list -> products[i].quantity_ = 0;
            memset(price_list->products[i].description_, 0, MAXTEXT);
        }
	}
}

// выводит все товары в терминал
void print_products(const Product_list *product_list) 
{
	if (list_is_empty(product_list)) 
	{
        printf("Список товаров пуст.\n");
        return;
    }
	
    for (size_t i = 0; i < product_list->capacity; i++) 
	{
        if (product_list->flags_array[i] == OCCUPIED) 
		{
            printf("Название: %s\n", product_list->products[i].name_);
            printf("Цена: %.2f\n", product_list->products[i].cost_);
            printf("Количество: %zu\n", product_list->products[i].quantity_);
            printf("Описание: %s\n\n", product_list->products[i].description_);
			printf("----------------------------\n");
        }
    }
}

// выводит товар в терминал
void print_product(const Product_list *product_list, const char *name) 
{
    if (list_is_empty(product_list)) 
	{
        printf("Список товаров пуст.\n");
        return;
    }

    int index = find_product_index(product_list, name);
	
    if (index == PROD_NOT_FOUND) 
	{
        printf("Товар с именем '%s' не найден.\n", name);
        return;
    }

    Product product = product_list->products[index];
    printf("Название: %s\n", product.name_);
    printf("Цена: %.2f\n", product.cost_);
    printf("Количество: %zu\n", product.quantity_);
    printf("Описание: %s\n", product.description_);
}


// удаления листа товаров, последняя применяется в работе
// пример product_list_del(price_list);
void product_list_del(Product_list * price_list) 
{
	//освобождаем сначала поля потом уже саму структуру
    free(price_list -> flags_array);
    free(price_list -> products);
    free(price_list);
}

