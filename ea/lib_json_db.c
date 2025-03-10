#include "lib_json_db.h"

// сохранение листа товаров в db
void save_products_to_json(Product_list *price_list, const char *filename) 
{
	// конвертируем Product_list в json_string
    char *json_string = convert_product_list_to_json_string(price_list);
	
	// записываем данные в файл
    if (json_string) 
	{
        FILE *file = fopen(filename, "w");
		
        if (file) 
		{
            fprintf(file, "%s", json_string);
            fclose(file);
        }
		
        free(json_string);
    }
}

// загрузка листа товаров из json db
int load_products_from_json(Product_list **price_list, const char *filename) 
{
	// удаление листа товаров старого 
	product_list_del(*price_list);
	
	// инициализация листа товаров
	*price_list = product_list_init();
	
	// загружаем строку из json
    FILE *file = fopen(filename, "r");
    if (!file) 
	{
        // Не удалось открыть файл
        return LIB_JSON_DB_ERROR;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char *json_string = malloc(length + 1);
    size_t result = fread(json_string, 1, length, file);
	if (result != length) 
	{
		return LIB_JSON_DB_ERROR;
	}


    json_string[length] = '\0'; // Завершаем строку нулевым символом
    fclose(file);

	// парсим json строку добавляя товары в лист товаров
    parse_json_to_product_list(*price_list, json_string);
    free(json_string);
	
	return EXIT_SUCCESS;
}

// конвертация листа товаров в json-строку
char* convert_product_list_to_json_string(Product_list *price_list) 
{
	// создаём массив JSON 
    cJSON *json_array = cJSON_CreateArray();
    
	// проходимся по листу товаров и выбираем те которые заполнены
    for (size_t i = 0; i < price_list->capacity; ++i) 
	{
		
		//каждый заполенный товар из листа товаров добавляем в JSON массив 
        if (price_list->flags_array[i] == OCCUPIED) 
		{
            cJSON *json_product = cJSON_CreateObject();
            cJSON_AddStringToObject(json_product, "name", price_list->products[i].name_);
            cJSON_AddNumberToObject(json_product, "cost", price_list->products[i].cost_);
            cJSON_AddNumberToObject(json_product, "quantity", price_list->products[i].quantity_);
            cJSON_AddStringToObject(json_product, "description", price_list->products[i].description_);
            cJSON_AddItemToArray(json_array, json_product);
        }
    }

	// создаём один общий объект товары, который содержит в себе массив товаров 
    cJSON *json_output = cJSON_CreateObject();
    cJSON_AddItemToObject(json_output, "products", json_array);
	
	// создаём json-строку из нашего объекта товары, содержащего массив товаров
    char *json_string = cJSON_Print(json_output);
    cJSON_Delete(json_output);
    
	// освободить память после использования
    return json_string; 
}

// парсинг json-строки в лист товаров: просто создание листа товаров из json-строки
int parse_json_to_product_list(Product_list *price_list, const char *json_string) 
{
	// преобразование строки JSON в объект cJSON
    cJSON *json = cJSON_Parse(json_string);
    if (json == NULL) 
	{
        return LIB_JSON_DB_ERROR;
    }

	// получение массива продуктов: извлекается массив продуктов из объекта JSON
    cJSON *json_products = cJSON_GetObjectItem(json, "products");
    if (json_products == NULL) 
	{
        // Нет массива продуктов в JSON
        cJSON_Delete(json);
        return LIB_JSON_DB_ERROR;
    }

	// Итерируемся по продуктам: используется макрос cJSON_ArrayForEach для перебора каждого продукта в массиве 
	// Для каждого продукта извлекаются его имя, стоимость, количество и описание
    cJSON *json_product;
    cJSON_ArrayForEach(json_product, json_products) 
	{
        const char *name = cJSON_GetObjectItem(json_product, "name")->valuestring;
        float cost = (float)cJSON_GetObjectItem(json_product, "cost")->valuedouble;
        size_t quantity = (size_t)cJSON_GetObjectItem(json_product, "quantity")->valueint;
        const char *description = cJSON_GetObjectItem(json_product, "description")->valuestring;

		// добавляем полученный товар в лист товаров
        add_product(price_list, name, cost, quantity, description);
    }

	// освобождается память, выделенная для объекта JSON
    cJSON_Delete(json);
}

// Парсинг JSON-строки в структуру Request
int parse_json_to_request(const char *json_str, Request *request) 
{
    if (json_str == NULL) 
    {
        return -1; // Ошибка: неверные аргументы
    }

    strcpy(request->json_str, json_str);
    //int length = strlen(request->json_str) - 1;
    //request->json_str[length] = '\0';

    cJSON *root = cJSON_Parse(json_str);
    if (root == NULL) 
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) 
        {
            //fprintf(stderr, "Ошибка парсинга JSON: %s\n", error_ptr);
        }
        return -1; // Ошибка парсинга
    }

    cJSON *name_request = cJSON_GetObjectItemCaseSensitive(root, "name_request");
    if (cJSON_IsString(name_request) && (name_request->valuestring != NULL)) 
    {
        strncpy(request->name_request, name_request->valuestring, MAXLINE - 1);
        request->name_request[MAXLINE - 1] = '\0';
    } 
    else 
    {
        cJSON_Delete(root);
        return -2; // Ошибка: поле "name_request" отсутствует или не является строкой
    }

    cJSON *product = cJSON_GetObjectItemCaseSensitive(root, "product");
    if (product != NULL) 
    {
        cJSON *name = cJSON_GetObjectItemCaseSensitive(product, "name");
        cJSON *cost = cJSON_GetObjectItemCaseSensitive(product, "cost");
        cJSON *quantity = cJSON_GetObjectItemCaseSensitive(product, "quantity");
        cJSON *description = cJSON_GetObjectItemCaseSensitive(product, "description");

        if (cJSON_IsString(name) && (name->valuestring != NULL)) 
        {
            strncpy(request->product.name_, name->valuestring, MAXLINE - 1);
            request->product.name_[MAXLINE - 1] = '\0';
        }
        if (cJSON_IsNumber(cost)) 
        {
            request->product.cost_ = (float)cJSON_GetNumberValue(cost);
        }
        if (cJSON_IsNumber(quantity)) 
        {
            request->product.quantity_ = (size_t)cJSON_GetNumberValue(quantity);
        }
        if (cJSON_IsString(description) && (description->valuestring != NULL)) 
        {
            strncpy(request->product.description_, description->valuestring, MAXTEXT - 1);
            request->product.description_[MAXTEXT - 1] = '\0';
        }
    } 
    else 
    {
        cJSON_Delete(root);
        return -3; // Ошибка: поле "product" отсутствует
    }

    // Парсим поле "client_sock"
    cJSON *client_sock = cJSON_GetObjectItemCaseSensitive(root, "client_sock");
    if (client_sock != NULL && cJSON_IsNumber(client_sock)) 
    {
        request->client_sock = (int)cJSON_GetNumberValue(client_sock);
    } 
    else 
    {
        cJSON_Delete(root);
        return -4; // Ошибка: поле "client_sock" отсутствует или не является числом
    }

    cJSON_Delete(root);
    return 0; // Успешный парсинг
}

// парсинг json-строку в ответ
Response parse_response(const char* json_str) 
{
    Response res;
    cJSON* json = cJSON_Parse(json_str);
    
    if (json) 
	{
        cJSON* response = cJSON_GetObjectItem(json, "response");
        
        strncpy(res.response, response->valuestring, MAXLINE);
        //res.json_str = strdup(json_str);
        
        cJSON_Delete(json);
    }
    return res;
}

// Функция для формирования JSON-строки из запроса
char *request_to_json(const Request *request) 
{
	// Проверка на NULL
    if (request == NULL) 
    {
        return NULL;
    }
	
    // Создаем корневой объект JSON
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) 
    {
        return NULL;
    }

    // Добавляем name_request
    if (request->name_request != NULL) 
    {
        cJSON_AddStringToObject(root, "name_request", request->name_request);
    }

    cJSON *product = cJSON_CreateObject();
    if (product != NULL) 
    {
        if (request->product.name_ != NULL) 
        {
            cJSON_AddStringToObject(product, "name", request->product.name_);
        }
        cJSON_AddNumberToObject(product, "cost", request->product.cost_);
        cJSON_AddNumberToObject(product, "quantity", request->product.quantity_);
        if (request->product.description_ != NULL) 
        {
            cJSON_AddStringToObject(product, "description", request->product.description_);
        }
        cJSON_AddItemToObject(root, "product", product);
    }
    
    // Добавляем информацию о клиенте
    cJSON *client = cJSON_CreateObject();
    if (client != NULL) 
    {
        cJSON_AddNumberToObject(client, "client_sock", request->client_sock);
        cJSON_AddItemToObject(root, "client", client);
    }

    // Преобразуем JSON-объект в строку
    char *json_str = cJSON_Print(root);
    if (json_str == NULL) 
    {
        cJSON_Delete(root);
        return NULL;
    }

    // Проверяем, что JSON-строка не превышает выделенный размер буфера
    size_t json_len = strlen(json_str);
    if (json_len >= MAXREQUEST) 
    {
        free(json_str); // Освобождаем память, выделенную cJSON
        cJSON_Delete(root);
        return NULL; // JSON-строка слишком большая
    }

    // Освобождаем память, выделенную под JSON
    cJSON_Delete(root);
    return json_str;
}

// Функция для парсинга ответа в JSON
char* parse_response_to_json(Response* res) 
{
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "response", res->response);
    
    //res->json_str = cJSON_Print(json);
    cJSON_Delete(json);
    return res->json_str;
}