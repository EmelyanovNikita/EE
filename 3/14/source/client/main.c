#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>


#define PORT 15645
#define BUFFER_SIZE 150


int main (int argc, char * argv[])
{
	int sockfd;
	struct sockaddr_in serveraddr;
	char buffer_in[BUFFER_SIZE], buffer_out[BUFFER_SIZE];
	
	
	//создание сокета
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
	
	
	//настройка сервера
	memset(&serveraddr, 0, sizeof(serveraddr));
	struct in_addr addr;
	serveraddr.sin_family = AF_INET; // семейство адресов
    if(serveraddr.sin_addr.s_addr = inet_aton("127.0.0.1", &addr) <= 0)
	{
		perror("inet_aton");
        exit(EXIT_FAILURE);
    }

    serveraddr.sin_port = htons(PORT); // номер порта   
	
	
	
	int len = sizeof(serveraddr); // длина датаграммы
	printf("Клиент работает\n");
	//отправляем и случаем сервер
    while (1) {
		//отправляем сообщение серверу
		printf("Напишите сообщение серверу: ");
		if ( (fgets(buffer_out, BUFFER_SIZE, stdin)) == NULL )
		{
			perror("fgets");
			exit(EXIT_FAILURE);
		}
		
		
		if((sendto(sockfd, buffer_out, strlen(buffer_out), 0, (const struct sockaddr *)&serveraddr, len)) < 0) 
		{
			perror("sendto");
			exit(EXIT_FAILURE);
		}
		if ((strcmp(buffer_out, "exit\n")) == 0) break;
		
		
		//получаем сообщение от сервера
        int n = recvfrom(sockfd, buffer_in, BUFFER_SIZE, 0, NULL, NULL);
		if ( n < 0 )
		{
			perror("recvfrom");
			exit(EXIT_FAILURE);
		}
		buffer_in[n] = '\0';
        printf("Получено сообщение от сервера: %s\n", buffer_in);
		if ((strcmp(buffer_in, "exit\n")) == 0) break;
    }


	if(close(sockfd) == -1)
	{
		perror("close");
		exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
	
	
    return 0;
}


