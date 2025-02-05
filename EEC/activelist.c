#include "activelist.h"

// Инициализация списка клиентов
ActiveList *active_list_init() 
{
    ActiveList *active_list = malloc(sizeof(ActiveList));
	
    if (active_list == NULL) 
	{
        return NULL;
    }
	
    active_list->size = active_list->max_size = active_list->capacity = 0;
    active_list->flags_array = NULL;
    active_list->client_fd  = NULL;
	
    return active_list;
}

// Хэш-функция djb2
size_t hash_funk_1(const char *str) 
{
    size_t hash = 5381;
    int c;
	
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	
    return hash;
}

// Хэш-функция sdbm
size_t hash_funk_2(const char *str) 
{
    size_t hash = 0;
    int c;
	
    while ((c = *str++))
        hash = c + (hash << 6) + (hash << 16) - hash;
	
    return hash;
}

// Проверка, пуст ли список клиентов
bool active_list_is_empty(const ActiveList *active_list)
{
    return active_list->size == 0;
}

// Поиск индекса клиента по client_fd
int find_client_index(const ActiveList *active_list, int client_fd) 
{
    if (active_list_is_empty(active_list)) return ACTIVE_LIST_EMPTY;
	
    size_t mask = active_list->capacity - 1;
	
	char str[20];
    // Получаем адрес переменной и форматируем его в строку
    sprintf(str, "%d", client_fd);
	
    size_t x = hash_funk_1(str) & mask;
    size_t y = hash_funk_2(str) & mask;
    size_t iter = 0;
	
	// максимальное количество итераций это размер таблицы умноженный на два
    size_t max_iter = active_list->capacity * 2;
	
    while (active_list->flags_array[x] != FREE && iter <= max_iter) 
	{
        if (active_list->flags_array[x] == OCCUPIED && active_list->client_fd[x] == client_fd) 
		{
            return x;
        }
		
        ++iter;
        x = (x + iter * y) & mask;
    }
	
    return CLIENT_NOT_FOUND;
}

// Поиск индекса клиента по индексу: тк у нас хэш таблица индекс это номер найденного ( первый найденный, второй, третий и тд )
int get_client_by_id(const ActiveList* active_list, size_t target_index)
{
	// проверка списка на пустоту
	if (active_list_is_empty(active_list)) return ACTIVE_LIST_EMPTY;
	
	// проверка на правильность переданного индекса
	if (target_index > (active_list->size))
	{
		// возврат NULL в случае если переданный индекс больше чем size (количество занятых элементов в хеш-таблице)
		return ACTIVE_LIST_ERROR;
	}
	
	size_t index_found_client = 0;
	
	for (size_t i = 0; i < (active_list->capacity) ; ++i)
	{
		if ( active_list->flags_array[i] == OCCUPIED )
		{
			++index_found_client;
			if (index_found_client == target_index)
			{
				return active_list->client_fd[i];
			}
		}
		
		
	}
	
	return CLIENT_NOT_FOUND;
}

// Добавление клиента
int add_client(ActiveList* active_list, int client_fd) 
{
    int res = find_client_index(active_list, client_fd);
    if (res != ACTIVE_LIST_EMPTY && res != CLIENT_NOT_FOUND) 
	{
        return CLIENT_ALREADY_EXISTS;
    }
    if (active_list->size >= active_list->max_size) 
	{
        size_t new_capacity = (active_list->capacity == 0) ? 8 : (active_list->capacity * 15) / 10;
        if (resize_active_list(active_list, new_capacity) == ACTIVE_LIST_ERROR) 
		{
            return ACTIVE_LIST_ERROR;
        }
    }
    size_t mask = active_list->capacity - 1;
	
	char str[20];
    // Получаем адрес переменной и форматируем его в строку
    sprintf(str, "%d", client_fd);
	
    size_t x = hash_funk_1(str) & mask;
    size_t y = hash_funk_2(str) & mask;
    
	for (size_t i = 0; i < active_list->capacity; ++i) 
	{
        if (active_list->flags_array[x] != OCCUPIED) 
		{
            active_list->flags_array[x] = OCCUPIED;
            active_list->client_fd[x] = client_fd;
            ++active_list->size;
            return x;
        }
        x = (x + i * y) & mask;
    }
    
	return CLIENT_NOT_ADDED;
}

// Изменение клиента
int change_client(ActiveList* active_list, int old_fd, int new_fd)
{
	// а существует ли тот которого хотим изменить
    int index = find_client_index(active_list, old_fd);
    if (index == ACTIVE_LIST_EMPTY || index == CLIENT_NOT_FOUND) 
	{
        return CLIENT_NOT_FOUND;
    }
	
	//изначально добавляем нового клиента
	index = add_client(active_list, new_fd);
	if (index == CLIENT_ALREADY_EXISTS) 
	{
        return CLIENT_ALREADY_EXISTS;
    }
	else if (index == ACTIVE_LIST_ERROR)
	{
		return ACTIVE_LIST_ERROR;
	}
	else if (index == CLIENT_NOT_ADDED)
	{
		return CLIENT_NOT_CHANGED;
	}
	
	// если клиент успешко добавиося удаляем клиента со старым значением сокета
    del_client(active_list, old_fd);
	
    return index;
}

// Удаление клиента
int del_client(ActiveList* active_list, int client_fd) 
{
    int index = find_client_index(active_list, client_fd);
	
    if (index == ACTIVE_LIST_EMPTY || index == CLIENT_NOT_FOUND) 
	{
        return CLIENT_NOT_FOUND;
    }
	
    active_list->flags_array[index] = DELETED;
    --active_list->size;
	
    return index;
}

// Увеличение размера таблицы
int resize_active_list(ActiveList *active_list, size_t new_capacity) 
{
    if (active_list->capacity < active_list->max_size) 
	{
        return 0;
    }
	
    new_capacity = round_to_pow_two(new_capacity);
    
	if (new_capacity <= active_list->capacity) 
	{
        new_capacity <<= 1;
    }
	
    Flags *new_flags_array = malloc(sizeof(Flags) * new_capacity);
    
	if (!new_flags_array) 
	{
        return ACTIVE_LIST_ERROR;
    }
	
    int* new_client_fd = malloc(sizeof(int) * new_capacity);
    
	if (!new_client_fd) 
	{
        free(new_flags_array);
        return ACTIVE_LIST_ERROR;
    }
	
    for (size_t i = 0; i < new_capacity; ++i) 
	{
        new_flags_array[i] = FREE;
    }
    size_t mask = new_capacity - 1;
    for (size_t i = 0; i < active_list->capacity; ++i) 
	{
        if (active_list->flags_array[i] != OCCUPIED) continue;
        
		char str[20];
		// Получаем адрес переменной и форматируем его в строку
		sprintf(str, "%d", active_list->client_fd[i]);
		
		size_t x = hash_funk_1(str) & mask;
		size_t y = hash_funk_2(str) & mask;
        size_t iter = 0;
       
	   while (new_flags_array[x] != FREE) 
		{
            ++iter;
            x = (x + iter * y) & mask;
        }
		
        new_flags_array[x] = OCCUPIED;
        new_client_fd[x] = active_list->client_fd[i];
    }
	
	free(active_list->flags_array);
    free(active_list->client_fd);
	
    active_list->flags_array = new_flags_array;
    active_list->client_fd = new_client_fd;
    active_list->capacity = new_capacity;
    active_list->max_size = (new_capacity >> 1) + (new_capacity >> 2);
	
    return 0;
}

// Округление до ближайшей степени двойки
size_t round_to_pow_two(size_t num) 
{
    if (num == 0) return 1;
    num--;
    num |= num >> 1;
    num |= num >> 2;
    num |= num >> 4;
    num |= num >> 8;
    num |= num >> 16;
    return num + 1;
}

// Очистка списка клиентов
void active_list_clear(ActiveList *active_list) 
{
    active_list->size = 0;
    if (active_list->flags_array && active_list->client_fd) 
	{
        for (size_t i = 0; i < active_list->capacity; ++i) 
		{
            active_list->flags_array[i] = FREE;
            active_list->client_fd[i] = -1;
        }
    }
}

// Вывод всех клиентов
void print_clients(const ActiveList *active_list) 
{
	printf("Все активные клиенты:\n");
    if (active_list_is_empty(active_list)) 
	{
        printf("Список клиентов пуст.\n");
        return;
    }
    for (size_t i = 0; i < active_list->capacity; i++) 
	{
        if (active_list->flags_array[i] == OCCUPIED) 
		{
            printf("Клиент: fd = %d\n", active_list->client_fd[i]);
        }
    }
}

// Вывод информации о конкретном клиенте
void print_client(const ActiveList* active_list, int client_fd)
{
    if (active_list_is_empty(active_list)) 
	{
        printf("Список клиентов пуст.\n");
        return;
    }
	
    int index = find_client_index(active_list, client_fd);
	
    if (index == CLIENT_NOT_FOUND) 
	{
        printf("Клиент с fd = %d не найден.\n", client_fd);
        return;
    }
	
    printf("Клиент: fd = %d\n", active_list->client_fd[index]);
}

// Удаление списка клиентов
void active_list_del(ActiveList *active_list) 
{
    free(active_list->flags_array);
    free(active_list->client_fd);
    free(active_list);
}