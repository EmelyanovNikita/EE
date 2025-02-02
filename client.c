#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8077
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Создание сокета
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Ошибка создания сокета");
        return 1;
    }

    // Настройка адреса сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    // Подключение к серверу
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Ошибка подключения к серверу");
        close(sock);
        return 1;
    }

    while (1) {
        printf("Введите сообщение (или 'exit' для выхода): ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // Удаление символа новой строки

        if (strcmp(buffer, "exit") == 0) {
            break; // Выход из цикла
        }

        // Отправка сообщения на сервер
        send(sock, buffer, strlen(buffer), 0);

        // Получение ответа от сервера
        int bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0'; // Завершение строки
            printf("Ответ от сервера: %s\n", buffer);
        } else {
            perror("Ошибка получения ответа");
            break;
        }
    }

    // Закрытие сокета
    close(sock);
    return 0;
}