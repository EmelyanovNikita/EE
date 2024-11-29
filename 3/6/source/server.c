#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define BUF_SIZE 100
#define NAME_SERVER_KEY "file"
#define PROJ_ID 3
#define QUEUE_PERMISSIONS 0660

typedef struct MESSAGE {
    long message_type;
    char buf[BUF_SIZE];
} MESSAGE;

int main(int argc, char **argv) {
    key_t msg_queue_key;
    int qid;
    MESSAGE message;
    MESSAGE return_message;

    if ((msg_queue_key = ftok(NAME_SERVER_KEY, PROJ_ID)) == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    if ((qid = msgget(msg_queue_key, IPC_CREAT | QUEUE_PERMISSIONS)) == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
    
    message.message_type = 2;
    printf("Сервер начал работу\n");

    while (1) {
        if (msgrcv(qid, &return_message, sizeof(char) * BUF_SIZE, 1, 0) == -1) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }

        if (strcmp(return_message.buf, "exit") == 0)
		break;
        

        printf("Сообщение от клиента: %s\n\n", return_message.buf);
 
        printf("Сервер напишите сообщение: ");
        fgets(message.buf, BUF_SIZE, stdin);

	int length = strlen(message.buf);
        if (message.buf[length - 1] == '\n')
            message.buf[length - 1] = '\0';

        if (strcmp(message.buf, "exit") == 0) {
            msgsnd(qid, &message, sizeof(char) * BUF_SIZE, 0);
            break;
        }


        if (msgsnd(qid, &message, sizeof(char) * BUF_SIZE, 0) == -1) {
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }
    }

    if (msgctl(qid, IPC_RMID, NULL) == -1) {
        perror("server: msgctl");
        exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
}

