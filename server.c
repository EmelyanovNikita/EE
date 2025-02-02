#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#define MAX_EVENTS 1000
#define MAX_SOCKETS 10
#define QUEUE_SIZE 10

typedef struct {
    char* messages[QUEUE_SIZE];
    int front, rear;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} MessageQueue;

MessageQueue queue = {.front = 0, .rear = 0};
pthread_mutex_t lock;

void enqueue(MessageQueue* q, char* message) {
    pthread_mutex_lock(&q->lock);
    while ((q->rear + 1) % QUEUE_SIZE == q->front) {
        pthread_cond_wait(&q->not_full, &q->lock);
    }
    q->messages[q->rear] = message;
    q->rear = (q->rear + 1) % QUEUE_SIZE;
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->lock);
}

char* dequeue(MessageQueue* q) {
    pthread_mutex_lock(&q->lock);
    while (q->front == q->rear) {
        pthread_cond_wait(&q->not_empty, &q->lock);
    }
    char* message = q->messages[q->front];
    q->front = (q->front + 1) % QUEUE_SIZE;
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->lock);
    return message;
}

int fd[MAX_SOCKETS]; // Массив сокетов
int n_sockets = 0; // Количество сокетов
pthread_mutex_t lock; // Мьютекс для защиты доступа к сокетам

void* accept_clients(void* arg) {
    int server_fd = *((int*)arg);
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int epfd = epoll_create1(0);
    
    while (1) {
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        pthread_mutex_lock(&lock);
        if (n_sockets < MAX_SOCKETS) {
            fd[n_sockets++] = client_fd;
            struct epoll_event ev;
            ev.data.fd = client_fd;
            ev.events = EPOLLIN;
            epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);
        } else {
            close(client_fd);
        }
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void* message_dispatcher(void* arg) {
    while (1) {
        char* message = dequeue(&queue);
        pthread_mutex_lock(&lock);
        for (int i = 0; i < n_sockets; ++i) {
            send(fd[i], message, strlen(message), 0);
        }
        pthread_mutex_unlock(&lock);
        free(message);
    }
    return NULL;
}

void* listen_sockets(void* arg) {
    int epfd = epoll_create1(0);
    struct epoll_event ev, events[MAX_EVENTS];

    for (int i = 0; i < n_sockets; ++i) {
        ev.data.fd = fd[i];
        ev.events = EPOLLIN;
        epoll_ctl(epfd, EPOLL_CTL_ADD, fd[i], &ev);
    }

    while (1) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            continue;
        }

        for (int i = 0; i < nfds; ++i) {
            char buf[1024];
            int n = recv(events[i].data.fd, buf, sizeof(buf), 0);
            if (n > 0) {
                buf[n] = '\0';
                enqueue(&queue, strdup(buf));
                printf("Received data: %s\n", buf);
            } else if (n == -1 && errno != EAGAIN) {
                perror("recv");
            }
        }
    }
    return NULL;
}

int main() {
    pthread_mutex_init(&queue.lock, NULL);
    pthread_cond_init(&queue.not_empty, NULL);
    pthread_cond_init(&queue.not_full, NULL);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);
    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, SOMAXCONN);
    
    pthread_t accept_thread, listen_thread, dispatcher_thread;
    pthread_mutex_init(&lock, NULL);

    pthread_create(&dispatcher_thread, NULL, message_dispatcher, NULL);
    pthread_create(&accept_thread, NULL, accept_clients, &server_fd);
    pthread_create(&listen_thread, NULL, listen_sockets, NULL);

    pthread_join(accept_thread, NULL);
    pthread_join(listen_thread, NULL);
    pthread_join(dispatcher_thread, NULL);

    pthread_mutex_destroy(&lock);
    return 0;
}