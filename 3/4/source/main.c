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
	
	
	pid_t pid = fork();
	int rv;
	for (int i = 0; i < num; ++i)
	{
		switch ( pid )
		{
			case -1:
			{
				perror("Ошибка, pid == -1\n");
				exit(EXIT_FAILURE);
			}
			case 0: // потомок
			{
				
				sleep(1); 
				printf("Потомок: ");
				if ( (child_fifo=open("./nam.1", O_WRONLY))  == -1 )
				{
					perror("Невозможно открыть fifo для записи\nОшибка");
					exit(0);
				}
				
				int value = rand() % 100;
				char buf[100];
				snprintf(buf, sizeof(buf), "%d", value);
				
				
				//записываем в канал
				if ( write(child_fifo, buf, sizeof(buf)) == -1)
				{
					perror("Невозможно записать\nОшибка");
				}
				else
					printf("Цифра: %d\n", value);
				
				
				close(child_fifo);
				if ( i == num -1) exit(EXIT_SUCCESS);
				else break;
				
			}
			default: // родитель
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
					printf("Прочитано из FIFO: %s\n", buf);
				//именно дописывает в файл, или передавать не существующий файл или удалять существующий
				FILE *fptr = fopen("b.txt", "a+"); 
				if (fptr == NULL) break;
				fprintf(fptr,"%s\n", buf);
				fclose(fptr);
				
				close(parent_fifo);
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

