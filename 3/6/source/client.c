#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define NAME_SERVER_KEY "file"
#define PROJ_ID 3
#define BUF_SIZE 100


typedef struct message {
    long message_type;
    char buf[BUF_SIZE];
} MESSAGE;

int main(int argc, char **argv) {
    key_t server_queue_key;
    int server_qid;
    MESSAGE my_message;
    MESSAGE return_message;

    if ((server_queue_key = ftok(NAME_SERVER_KEY, PROJ_ID)) == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    if ((server_qid = msgget(server_queue_key, 0)) == -1) {
        perror("msgget: server_qid");
        exit(EXIT_FAILURE);
    }

   my_message.message_type = 1;  

    printf("Пожалуйста введите сообщение: ");

    while (1) {
	
	fgets(my_message.buf, BUF_SIZE, stdin);
        int length = strlen(my_message.buf);
        if (my_message.buf[length - 1] == '\n')
            my_message.buf[length - 1] = '\0';

        if (strcmp(my_message.buf, "exit") == 0) {
            msgsnd(server_qid, &my_message, sizeof(char) * BUF_SIZE, 0);
            break;
        }

        if (msgsnd(server_qid, &my_message, sizeof(char) * BUF_SIZE, 0) == -1) {
            perror("client: msgsnd");
            exit(EXIT_FAILURE);
        }

        if (msgrcv(server_qid, &return_message, sizeof(char) * BUF_SIZE, 2, 0) == -1) {
            perror("client: msgrcv");
            exit(EXIT_FAILURE);
        }

	if (strcmp(return_message.buf, "exit") == 0) 
		break;
        

        printf("Сообщение полученное от сервера: %s\n\n", return_message.buf);
        printf("Пожалуйста введите сообщение: ");
    }


    exit(EXIT_SUCCESS);
}
