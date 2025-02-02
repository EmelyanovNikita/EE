# Определение компилятора и флагов
CC = gcc
CFLAGS = -Wall -Wextra

# Исходные файлы
SERVER_SRC = server.c
CLIENT_SRC = client.c

# Исполняемые файлы
SERVER_BIN = server
CLIENT_BIN = client

# Правила сборки
all: $(SERVER_BIN) $(CLIENT_BIN)

$(SERVER_BIN): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $@ $^

$(CLIENT_BIN): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $@ $^

# Очистка
clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN)