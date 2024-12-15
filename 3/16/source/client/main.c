#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define str1 "Enter filename\r\n"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int my_sock, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

	char buff[1024];
    printf("TCP DEMO CLIENT\n");
	
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    // извлечение порта
	portno = atoi(argv[2]);
    
	// Шаг 1 - создание сокета
	my_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (my_sock < 0) 
        error("ERROR opening socket");
    // извлечение хоста
	server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    // заполенние структуры serv_addr
	bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    // установка порта
	serv_addr.sin_port = htons(portno);
    
	// Шаг 2 - установка соединения	
	if (connect(my_sock,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    
	// Шаг 3 - чтение и передача сообщений
	while ((n = recv(my_sock, &buff[0], sizeof(buff) - 1, 0)) > 0)
     {
		
         // ставим завершающий ноль в конце строки
         buff[n] = '\0';

         // выводим на экран
         printf("S=>C:%s", buff);
         

         // проверка на то когда нам пора идти
		if ( !(strcmp(buff, str1) == 0) )
		{
			// Корректный выход
			printf("Exit\n");
			close(my_sock);
			return 0;
		}
		else
		{
			// читаем пользовательский ввод с клавиатуры
			printf("S<=C:"); 
			char name[1024];
			if ( (fgets(&name[0], sizeof(name) - 1, stdin)) == NULL) 
			{
				perror("fgets");
				exit(EXIT_FAILURE);
			}
			name[strcspn(name, "\n")] = 0; // Удаляем символ новой строки
			FILE *fp = fopen((const char *)name, "rb");
			if ( fp == NULL )
			{
				perror("fopen");
				exit(EXIT_FAILURE);
			}
			while (fread(buff, sizeof(char), sizeof(buff), fp) > 0) 
			{
				// передаем строку клиента серверу
				send(my_sock, &buff[0], strlen(&buff[0]), 0);
			}
			fclose(fp);
			
		}
     }
     printf("Recv error \n");
     close(my_sock);
     return -1;
}

