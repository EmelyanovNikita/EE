#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define FILENAME "file.txt"
#define KEY_FILENAME "f"
#define NUMOFPROC 8


union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

//создаём именованный канал и потом только уже создаём потомка и внутри каждого условий (для родителя и для потомка) выполняем действие переданное в аргументе количество раз
 int main (int argc, char * argv[])
{
	key_t key;
	int sem_id;
	union semun attr;
	//количсество рандомных чисел
	int num = atoi(argv[1]);
	
	//создаём переменные для канала
	int parent_fifo, child_fifo;
	srand(time(NULL));
	
	//проврека на количество переданных парматеров
	if ( argc < 2 ) 
	{
		printf("Ошибка, не было передано аргументов\n");
		printf("Вы должны передать 1 аргумент\n");
		exit(EXIT_FAILURE);
	}
	else if ( argc > 2)
	{
		printf("Ошибка, слишком много было передано аргументов\n");
		printf("Вы должны передать 1 аргумент\n");
		exit(EXIT_FAILURE);
	}
	
	if ((key = ftok(KEY_FILENAME, 3)) == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }
	 
	
    if( ( sem_id = semget(key, 1, IPC_CREAT | 0666 ) )== -1){
	perror("Ошибка семафора\n");
	exit(EXIT_FAILURE);
    }
    
	struct sembuf lock_w = {0,-NUMOFPROC,0};
	struct sembuf unlock_w = {0,NUMOFPROC,0};
    struct sembuf lock_r = {0,-1,0};
	struct sembuf unlock_r = {0,1,0};
	
	
    attr.val = NUMOFPROC;
	
	
    if(semctl(sem_id, 0, SETVAL, attr) == -1){
		perror("Ошибка semctl\n");
		exit(EXIT_FAILURE);
    }
	
	
	//отчищаем если такой уже существует
	unlink("nam.1");
	
	//создаём канал
	if ((mkfifo("./nam.1" , 0746)) == -1) // у вас на слайде здесь ошибка mkfifo("./n.1" , O_RDWR)
	{
		perror("Невозможно создать fifo\n");
		perror("Ошибка: ");
		exit(0);
	}
	
	
	pid_t pid = fork();
	for (int i = 0; i < num + 1; ++i)
	{
		switch ( pid )
		{
			//все хорошо осталось только сделать чтобы потомок считывал из файла информацию на один раз больше чем весь остальной цикл
			case -1:
			{
				perror("Ошибка, pid == -1\n");
				exit(EXIT_FAILURE);
			}
			case 0: // потомок
			{
				sleep(1);
				if ( i < num )
				{
					if ( (child_fifo=open("./nam.1", O_WRONLY))  == -1 )
					{
						perror("Невозможно открыть fifo для записи\nОшибка");
						exit(0);
					}
				}
				
				
				if ( i > 0)
				{
					if((semop(sem_id, &lock_r, 1)) == -1)
					{
						perror("Ошибка semop(sem_id, &lock)\n");
						exit(EXIT_FAILURE);
					}
					int buff;
					FILE *fp = fopen(FILENAME, "r");
					
					
					
					if (fp == NULL) {
						printf("Ошибка открытия файла\n");
						exit(EXIT_FAILURE);
					}
					if ( fscanf(fp,"%d\n", &buff) < 0 ) perror ("fscanf");
					
					printf("Потомок прочитал из файла: %d\n\n", buff);
					fclose(fp);
					if((semop(sem_id, &unlock_r, 1)) == -1)
					{
						perror("Ошибка semop(semop(sem_id, unlock, 1))\n");
						exit(EXIT_FAILURE);
					}
				}
				
				
				if ( i < num )
				{
					int value = rand() % 100;
					char buf[100];
					snprintf(buf, sizeof(buf), "%d", value);
					
					
					//записываем в канал
					if ( write(child_fifo, buf, sizeof(buf)) == -1)
					{
						perror("Невозможно записать\nОшибка");
					}
					else
						printf("Потмок: Цифра в канал: %d\n", value);
					close(child_fifo);
				}
				
				
				if ( i == num) exit(EXIT_SUCCESS);
				else break;
				
			}
			default: // родитель
			{
				if ( i < num )
				{
					printf("Родитель: ");
					char buf[100];
					//открываем канал только на чтение
					if ((parent_fifo=open("./nam.1" , O_RDONLY)) == -1)
					{
						fprintf(stderr, "Невозможно открыть fifo для чтения\n");
						exit(0);
					}
					FILE *fp = fopen(FILENAME, "w");
					if (fp == NULL) {
						printf("Ошибка открытия файла");
						exit(EXIT_FAILURE);
					}
					//читаем из канала
					if (read(parent_fifo, &buf, sizeof(buf)) == -1)
						fprintf(stderr, "Невозможно прочесть fifo\n");
					else
						printf("Прочитал из FIFO: %s\n", buf);
					
					if((semop(sem_id, &lock_w, 1)) == -1)
					{
						perror("Ошибка semop(sem_id, &lock)\n");
						exit(EXIT_FAILURE);
					}
					
					fprintf(fp, "%d\n", atoi(buf));
					
					if((semop(sem_id, &unlock_w, 1)) == -1)
					{
						perror("Ошибка semop(semop(sem_id, unlock, 2))\n");
						exit(EXIT_FAILURE);
					}
					
					printf("Родитель записал в файл: %s\n", buf);
					fclose(fp);
					
					
					
					close(parent_fifo);
				}
				
				
				break;
			}
			
		}
	}
	
	
	
	if ( pid == 0 ) exit(EXIT_SUCCESS);
	
	
	if (pid > 0) {
		int st;
		waitpid(pid, &st, 0);
	}
	if(semctl(sem_id, 0, IPC_RMID) < 0)
	{
		perror("Ошибка semctl(sem_id, 0, IPC_RMID)\n");
		exit(EXIT_FAILURE);
    }
	return 0;


}

