#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>


#define FILENAME "file.txt"


int file_access = 1;

void sigusr1_handler(int signo) {
    file_access = 0;
}

void sigusr2_handler(int signo) {
    file_access = 1;
}


//создаём именованный канал и потом только уже создаём потомка и внутри каждого условий (для родителя и для потомка) выполняем действие переданное в аргументе количество раз
 int main (int argc, char * argv[])
{
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
	
	//количсество рандомных чисел
	int num = atoi(argv[1]);
	
	//создаём переменные для канала
	int parent_fifo, child_fifo;
	
	//отчищаем если такой уже существует
	unlink("nam.1");
	
	//создаём канал
	if ((mkfifo("./nam.1" , 0746)) == -1) // у вас на слайде здесь ошибка mkfifo("./n.1" , O_RDWR)
	{
		perror("Невозможно создать fifo\n");
		perror("Ошибка: ");
		exit(0);
	}
	
	
	signal(SIGUSR1, sigusr1_handler);
    signal(SIGUSR2, sigusr2_handler);
	
	
	pid_t pid = fork();
	for (int i = 0; i < num + 1; ++i)
	{
		switch ( pid )
		{
			//все хорошо осталось только сделать чтобы потомк считывал из файла информацию на один раз больше чем весь остальной цикл
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
					printf("Потомок: ");
					if ( (child_fifo=open("./nam.1", O_WRONLY))  == -1 )
					{
						perror("Невозможно открыть fifo для записи\nОшибка");
						exit(0);
					}
				}
			
				if ( i > 0)
				{
					 while (!file_access)
                        pause();
					int buff;
					FILE *fp = fopen(FILENAME, "r");
					if (fp == NULL) {
						printf("Ошибка открытия файла\n");
						exit(EXIT_FAILURE);
					}
					fscanf(fp,"%d\n", &buff);
					printf("Потомок прочитал из файла: %d\n\n", buff);
					fclose(fp);
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
					
					
					//читаем из канала
					if (read(parent_fifo, &buf, sizeof(buf)) == -1)
						fprintf(stderr, "Невозможно прочесть fifo\n");
					else
						printf("Прочитал из FIFO: %s\n", buf);
					
					kill(pid, SIGUSR1); 
					FILE *fp = fopen(FILENAME, "w");
					if (fp == NULL) {
						printf("Ошибка открытия файла");
						exit(EXIT_FAILURE);
					}
					fprintf(fp, "%d\n", atoi(buf));
					printf("Родитель записал в файл: %s\n", buf);
					fclose(fp);
					kill(pid, SIGUSR2); 
					
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
	return 0;


}

