#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <signal.h>
#include <arpa/inet.h>


#define PORT 15645
#define BUFFER_SIZE 150


int sockfd;


void handle_sigint(int sig) {
    if (close(sockfd) == -1) {
        perror("close");
    }
    exit(EXIT_SUCCESS);
}


int main (int argc, char * argv[])
{
	struct sockaddr_in serveraddr, clientaddr;
	char buffer_in[BUFFER_SIZE], buffer_out[BUFFER_SIZE];
	socklen_t len;
	
	
	// Установка обработчика сигнала
    signal(SIGINT, handle_sigint);
	
	
	//создание сокета
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
	
	
	//настройка сервера
	memset(&clientaddr, 0, sizeof(clientaddr));
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET; // семейство адресов
    serveraddr.sin_addr.s_addr = INADDR_ANY;  // подключение со всех IP-адресов
    serveraddr.sin_port = htons(PORT);  // порт
	
	
	//связывание сокета
	if ( bind (sockfd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr))  != 0 )
	{
		perror("bind");
        exit(EXIT_FAILURE);
	}
	
	
	len = sizeof(clientaddr); // длина датаграммы
	//ожидаем и случаем клиента
	printf("Сервер работает\n");
    while (1) {
		//получаем сообщение от клиента
        int n = recvfrom(sockfd, buffer_in, BUFFER_SIZE, 0, (struct sockaddr *)&clientaddr, &len);
        if(n < 0)
		{
		   perror("recvform");
		   exit(EXIT_FAILURE);
		}
		buffer_in[n] = '\0';
        printf("Получено сообщение от клиента: %s\n", buffer_in);
		if ((strcmp(buffer_in, "exit\n")) == 0) break;
		
		
		//отправляем сообщение клиенту
		printf("Напишите ответ сервера: ");
		if ( (fgets(buffer_out, BUFFER_SIZE, stdin)) == NULL )
		{
			perror("fgets");
			exit(EXIT_FAILURE);
		}
		if((sendto(sockfd, buffer_out, strlen(buffer_out), 0, (const struct sockaddr *)&clientaddr, len)) < 0)
		{
		   perror("sendto");
		   exit(EXIT_FAILURE);
		}
		if ( strcmp(buffer_out, "exit\n") == 0 ) break;
    }


	if(close(sockfd) == -1)
	{
		perror("close");
		exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
	
	
    return 0;
}


