#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>
#include <semaphore.h>


#define FILENAME "file.txt"
#define SEMAPHORE_NAME "/my_semaphore"


//создаём именованный канал и потом только уже создаём потомка и внутри каждого условий (для родителя и для потомка) выполняем действие переданное в аргументе количество раз
 int main (int argc, char * argv[])
{
	sem_t *sem;
	//количсество рандомных чисел
	int num;
	//создаём переменные для канала
	int parent_fifo, child_fifo;
	pid_t pid;
	
	//для rand и лучшей генерации рамномных чисел
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
	
	//если был такой семафор unlink тогда 
	sem_unlink(SEMAPHORE_NAME);
	//создадим новый семафор с начальным значением 1 
	if ( (sem = sem_open(SEMAPHORE_NAME, O_CREAT, 0777, 1)) == SEM_FAILED ) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
	
	
	//отчищаем если такой уже существует
	unlink("nam.1");
	
	//создаём канал
	if ((mkfifo("./nam.1" , 0746)) == -1) // создаём именно именованный канал
	{
		perror("Невозможно создать fifo\n");
		perror("Ошибка: ");
		exit(0);
	}
	
	//преобразуем  в int наш переданный параметр
	num = atoi(argv[1]);
	//создадим новый дочерний процесс
	pid = fork();
	for (int i = 0; i < num + 1; ++i) //цикл проёдет на 1 операцию чем указали количество итераций так как для вывода 
	{								  //последнего значения записанного в файл необходима одна доп итерация
		switch ( pid )
		{
			
			case -1:
			{
				perror("Ошибка, pid == -1\n");
				exit(EXIT_FAILURE);
			}
			case 0: // потомок
			{
				if ( i > 0) // при чтении файла пропускаем первю итерацию тк тогда ещё не записана цифра в файл и считывать нечего ещё
				{
					//открываем файл 
					int buff;
					FILE *fp = fopen(FILENAME, "r");
					if (fp == NULL) {
						printf("Ошибка открытия файла\n");
						exit(EXIT_FAILURE);
					}
					//заблокируеум семафор
					if( sem_wait(sem) == -1 )
					{
						perror("sem_wait");
						exit(EXIT_FAILURE);
					}
					//пишем в файл
					if ( fscanf(fp,"%d\n", &buff) < 0 ) perror("fscanf");
					printf("Потомок прочитал из файла: %d\n\n", buff);
					
					//разблокируем семафор
					if ( sem_post (sem) == -1 )
					{
						perror("sem_post");
						exit(EXIT_FAILURE);
					}
					//закроем файл
					fclose(fp);
					
				}
				
				
				if ( i < num )// так как у нас цикл проходит + 1 итерацию для вывода последнего значения из файла, ограничим количество итераций для записи в канал дочерним процессом
				{
					//заблокируеум семафор
					if( sem_wait(sem) == -1 )
					{
						perror("sem_wait");
						exit(EXIT_FAILURE);
					}
					
					//открываем канал на запись
					if ( (child_fifo=open("./nam.1", O_WRONLY))  == -1 )
					{
						perror("Потомок: Невозможно открыть fifo для записи\nОшибка");
						exit(0);
					}
					
					
					int value = rand() % 100;
					char buf[100];
					snprintf(buf, sizeof(buf), "%d", value); //преносим данные в str
					
					
					//записываем в канал
					if ( write(child_fifo, buf, sizeof(buf)) == -1)
					{
						perror("Невозможно записать\nОшибка");
					}
					else
						printf("Потмок: Цифра в канал: %d\n", value);
					//разблокируем семафор
					if ( sem_post (sem) == -1 )
					{
						perror("sem_post");
						exit(EXIT_FAILURE);
					}
					//закроем канал
					close(child_fifo);
				}
				break;
				
			}
			default: // родитель
			{
					
				if ( i == 0 ) // нам надо чтобы потомок первым пришёл к очереди и записал в неё своё значение
				{ 			  // поэтому ждём пока семафор не заблокируется
					int val;
					//получаем значение семафора
					sem_getvalue(sem, &val);
					while ( val == 1){
						sem_getvalue(sem, &val);
						
					}
				}
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
					//откроем файл на запись
					FILE *fp = fopen(FILENAME, "w");
					if (fp == NULL) {
						printf("Ошибка открытия файла");
						exit(EXIT_FAILURE);
					}
					//заблокируем семафор
					if( sem_wait(sem) == -1 )
					{
						perror("sem_wait");
						exit(EXIT_FAILURE);
					}
					
					//читаем из канала
					if (read(parent_fifo, &buf, sizeof(buf)) == -1)
						fprintf(stderr, "Невозможно прочесть fifo\n");
					else
						printf("Прочитал из FIFO: %s\n", buf);
					
					//пишем в файл
					fprintf(fp, "%d\n", atoi(buf));
					printf("Родитель записал в файл: %s\n", buf);
					
					
					//разблокируем семафор
					if ( sem_post(sem) == -1 )
					{
						perror("sem_post");
						exit(EXIT_FAILURE);
					}
					// закрываем файл
					fclose(fp);
					//закрываем канал
					close(parent_fifo);
				}
				
				
				break;
			}
			
		}
	}
	
	// если ты потомок, выходим 
	if ( pid == 0 ) exit(EXIT_SUCCESS);
	
	//если родитель, ждём ребёнка дома
	if (pid > 0) {
		int st;
		waitpid(pid, &st, 0);
	}
	
	// закрываем семафор
	if ( sem_close(sem) == -1 )
	{
		perror("sem_close");
		exit(EXIT_FAILURE);
	}
	
	// unlink удаляем семафор
	if( sem_unlink(SEMAPHORE_NAME) == -1)
	{
		perror("sem_unlink");
		exit(EXIT_FAILURE);
	}
	
	
	return 0;
}

