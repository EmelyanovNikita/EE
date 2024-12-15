/* Пример простого TCP сервера
   Порт является аргументом, для запуска сервера неободимо ввести:
   # ./[имя_скомпилированного_файла] [номер порта]
   # ./server_exe.x 57123
   # ./client_exe.x 127.0.0.1 57123
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <semaphore.h>
#include <sys/mman.h>

#define str1 "Enter 1 parametr\r\n"
#define str2 "Enter 2 parametr\r\n"
#define str3 "Enter enter the operation\r\n"
#define SEMAPHORE_NAME "/my_semaphore"
#define POSIX_SHM_NAME "/shm_name_posix"


// Функция обработки ошибок
void error(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}


typedef struct Share_memory //храним всю основную информацию о нашей разделяемой памяти
{
	int shmem_id_; //id разделяемой памяти
	size_t	nclients; //количество активных пользователей 
} Share_memory;


// прототип функции, обслуживающей
// подключившихся пользователей
void dostuff(int sock, Share_memory *sh_mem, sem_t* sem);


// макрос для печати количества активных
// пользователей 
void printusers(Share_memory* sh_mem)
{ 
	if(sh_mem -> nclients) printf("%ld user on-line\n", sh_mem -> nclients);
	else {printf("No User on line\n");}
}

// функция по варианту
int myfunc_sum(int a, int b)
{
	return a+b;
}


int myfunc_sub(int a, int b){
   return a-b;
}

int myfunc_um(int a, int b){
   return (a*b);
}

int myfunc_dele(int a, int b){
	if ( b == 0) return 0;
	return a/b;
}

//тк не хочется множить код и писать много раз  одно и тоже вынесем блокировки и разюлокировку семафора в отдельную функцию
void sem_lock(sem_t* sem)
{
	// sem_wait (sem);
	if( sem_wait (sem) == -1 )
	{
		perror("sem_wait");
		// exit(EXIT_FAILURE);
	}
}
void sem_unlock(sem_t* sem)
{
	// sem_post (sem);
	if ( sem_post (sem) == -1 )
	{
		perror("sem_post");
		// exit(EXIT_FAILURE);
	}
}

int main(int argc, char *argv[])
{
    char buff[1024];
	printf("TCP SERVER DEMO\n");
	
	int sockfd, newsockfd; // дескрипторы сокетов
	int portno; // номер порта
	int pid; // id номер потока
    socklen_t clilen; // размер адреса клиента типа socklen_t
    struct sockaddr_in serv_addr, cli_addr; // структура сокета сервера и клиента
	
	
	//указатель на разделяемую память
	Share_memory *sh_mem;
	//придётся использовать временные переменные так как памяти изначально нет и некуда записать информацию 
	int shmem_id_temp; //временный ID для sh_mem
	
	
	//работа с семафором
	//переменные для создания семафоров
	sem_t *sem;
	//создадим новые семафоры с начальными значениями 1 (количество процессов)
	if ( (sem = sem_open(SEMAPHORE_NAME, O_CREAT, 0777, 1)) == SEM_FAILED ) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
	
	
	//работа с разделяемой памятью
	//если с таким именем была уже удалим её тогда, ошибку обработаем но завершать работу программы не будем тк это не проблема в данном моменте
	if (( shm_unlink(POSIX_SHM_NAME) ) == -1)
	{
		perror("shm_unlink");
		//exit(EXIT_FAILURE);
	}
	//создание разделяемой памяти
	if (( shmem_id_temp = shm_open (POSIX_SHM_NAME, O_RDWR | O_CREAT, 0664) ) == -1) 
	{
		perror("shm_open");
		exit(EXIT_FAILURE);
	}
	//установим длину разделяемой памяти в размере sizeof (struct Share_memory) 
	if ( ftruncate (shmem_id_temp, sizeof (struct Share_memory)) == -1)
	{
		perror("ftruncate");
		exit(EXIT_FAILURE);
	}
    //отразим в память 
	if (( sh_mem = mmap(NULL, sizeof (struct Share_memory), PROT_READ | PROT_WRITE, MAP_SHARED, shmem_id_temp, 0) ) == (Share_memory *) -1 )
    {
		perror("mmat");
		exit(EXIT_FAILURE);
	}
	
	
	//присваиваем элементам структуры значения временных переменных
	sh_mem->shmem_id_ = shmem_id_temp;
	sh_mem->nclients = 0;
	
	
	// ошибка в случае если мы не указали порт
    if (argc < 2) 
	{
        fprintf(stderr,"ERROR, no port provided\n");
        exit(EXIT_FAILURE);
    }
	
	
    // Шаг 1 - создание сокета
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // ошибка при создании сокета
	if (sockfd < 0) 
		error("ERROR opening socket");
	
	//close(sockfd);
	// устанавливаем SO_REUSEADDR опцию, необходимо для повторного подключения к существующему сокету
    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        perror("setsockopt");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
	
	
	// Шаг 2 - связывание сокета с локальным адресом
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; // сервер принимает подключения на все IP-адреса
    serv_addr.sin_port = htons(portno);
	
	
    // вызываем bind для связывания
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");
	
	
    // Шаг 3 - ожидание подключений, размер очереди - 5
	listen(sockfd, 5);
    clilen = sizeof(cli_addr);
	
	// Шаг 4 - извлекаем сообщение из очереди
	// цикл извлечения запросов на подключение из очереди    
	while (1) 
	{
		newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) 
			error("ERROR on accept");
		sem_lock(sem);
		sh_mem->nclients++; // увеличиваем счетчик
		sem_unlock(sem);
		
		printusers(sh_mem);

		pid = fork();
		if (pid < 0)
			error("ERROR on fork");
		if (pid == 0)  
		{
			close(sockfd);
			dostuff(newsockfd, sh_mem, sem);
			exit(EXIT_SUCCESS);
		}
		else close(newsockfd);
		
	}
	// Открепление разделяемой памяти
    if (munmap(sh_mem, sizeof (struct Share_memory)) == -1) {
        perror("munmap");
        exit(1);
    }
    // Удаление разделяемой памяти
	if (( shm_unlink(POSIX_SHM_NAME) ) == -1)
	{
		perror("shm_unlink");
		exit(EXIT_FAILURE);
	}
	// закрываем семафор
	if ( sem_close(sem) == -1 )
	{
		perror("sem_close");
		exit(EXIT_FAILURE);
	}
	// unlink удаляем семафоры
	if( sem_unlink(SEMAPHORE_NAME) == -1)
	{
		perror("sem_unlink");
		exit(EXIT_FAILURE);
	}
	close(sockfd);
	exit(EXIT_SUCCESS);
}

void dostuff (int sock, Share_memory *sh_mem, sem_t* sem)
{
	int bytes_recv; 								// размер принятого сообщения
	int a, b, result;	
	char operator;
	char buff[20 * 1024];
    
	
	memset(buff,0,sizeof(buff));
	// отправляем клиенту сообщение
   	if ( (write(sock, str1, sizeof(str1))) == -1) 
	{
		perror("write");
		exit(EXIT_FAILURE);
	}
   
   	// обработка первого параметра
	bytes_recv = read(sock,&buff[0],sizeof(buff));
	if (bytes_recv < 0) error("ERROR reading from socket");
	a = atoi(buff); 								// преобразование первого параметра в int
	
	// отправляем клиенту сообщение
	if ( (write(sock,str2,sizeof(str2))) == -1) 
	{
		perror("write");
		exit(EXIT_FAILURE);
	}
	// обработка второго параметра
	bytes_recv = read(sock,&buff[0],sizeof(buff));
	if (bytes_recv < 0) error("ERROR reading from socket");
	
	b = atoi(buff); 								// преобразование второго параметра в int
	
	
	// отправляем клиенту сообщение
	if ( (write(sock, str3, sizeof(str3))) == -1) 
	{
		perror("write");
		exit(EXIT_FAILURE);
	}
	// обработка оператора
	bytes_recv = read(sock, &operator, sizeof(operator));
	if (bytes_recv < 0) error("ERROR reading from socket");
	
	
	switch(operator){
		case '+':
		
		   result = myfunc_sum(a,b);
		   break;

		case '-':
		   result = myfunc_sub(a,b);
		   break;

		case '*':
		   result = myfunc_um(a,b);
		   break;

		case '/':
		   result = myfunc_dele(a,b);
		   break;

		default:
		   printf("Invalid operation!\n");
		   return;
	}
	
	
	memset(buff,0,sizeof(buff));
	// вызов пользовательской функции
	snprintf(buff, sizeof(buff), "%d", result); 			// преобразование результата выполнения в строку
	buff[strlen(buff)] = '\n'; 						// добавление к сообщению символа конца строки
	
	
	// отправляем клиенту результат
	if ( (write(sock,&buff[0],sizeof(buff))) == -1) 
	{
		perror("write");
		exit(EXIT_FAILURE);
	}
	
	sem_lock(sem);
	--(sh_mem->nclients);
	sem_unlock(sem);
	printf("-disconnect\n");
	printusers(sh_mem);
	return;
}

