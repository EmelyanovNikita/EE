#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <mqueue.h>

#define NAME_SERVER_QUEUE "/server_queue"
#define BUF_SIZE 100


int main(int argc, char **argv) {
    mqd_t server_queue_key;
    
    char in_buff[BUF_SIZE], out_buff[BUF_SIZE];


    if ((server_queue_key = mq_open(NAME_SERVER_QUEUE, O_RDWR)) == -1) {
        perror("Клиент: mq_open");
        exit(EXIT_FAILURE);
    }

    printf("Клиент пожалуйста введите сообщение: ");

    while (1) {
	
	fgets(out_buff, BUF_SIZE, stdin);
        int length = strlen(out_buff);
        if (out_buff[length - 1] == '\n')
            out_buff[length - 1] = '\0';

        if (mq_send(server_queue_key, out_buff, BUF_SIZE, 0) == -1) {
            perror("клиент: mq_send");
            exit(EXIT_FAILURE);
        }

	if (strcmp(out_buff, "exit") == 0) break;


        if (mq_receive(server_queue_key, in_buff, BUF_SIZE, NULL) == -1) {
            perror("клиент: mq_receive");
            exit(EXIT_FAILURE);
        }

	if (strcmp(in_buff, "exit") == 0) break;
        

        printf("Сообщение полученное от сервера: %s\n\n", in_buff);
        printf("Пожалуйста введите сообщение: ");
    }


    exit(EXIT_SUCCESS);
}
