#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <mqueue.h>

#define BUF_SIZE 100
#define NAME_SERVER_QUEUE "/server_queue"
#define QUEUE_PERMISSIONS 0660
#define MAX_MSG 10

int main(int argc, char **argv) {
    mqd_t msg_queue_key;


    char in_buff[BUF_SIZE], out_buff[BUF_SIZE];
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MSG;
    attr.mq_msgsize =  sizeof(out_buff);
    attr.mq_curmsgs = 0;


    if ((msg_queue_key = mq_open(NAME_SERVER_QUEUE, O_CREAT | O_RDWR, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror("server: mq_open");
        exit(EXIT_FAILURE);
    }


    printf("Сервер начал работу\n");

    while (1) {
        if (mq_receive(msg_queue_key, in_buff, BUF_SIZE, NULL) == -1) {
            perror("server: mq_receive");
            exit(EXIT_FAILURE);
        }

        if (strcmp(in_buff, "exit") == 0)
		break;
        

        printf("Сообщение от клиента: %s\n\n", in_buff);
 
        printf("Сервер напишите сообщение: ");
        fgets(out_buff, BUF_SIZE, stdin);

	int length = strlen(out_buff);
        if (out_buff[length - 1] == '\n')
            out_buff[length - 1] = '\0';


        if (mq_send(msg_queue_key, out_buff, BUF_SIZE, 0) == -1) {
            perror("server mq_send");
            exit(EXIT_FAILURE);
        }
	if (strcmp(out_buff, "exit") == 0) {
            break;
        }
    }

    if (mq_close(msg_queue_key) == -1) {
        perror("server: mq_close");
        exit(EXIT_FAILURE);
    }
    if (mq_unlink(NAME_SERVER_QUEUE) == -1){
	    perror("server: mq_unlink");
    }
    
    exit(EXIT_SUCCESS);
}

