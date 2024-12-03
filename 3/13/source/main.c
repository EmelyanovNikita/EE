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
#include <sys/ipc.h>
#include <semaphore.h>
#include <limits.h>
#include <math.h>
#include <sys/mman.h>


//определяем основные данные для инициализаци памяти и семафоров 
#define BUF_SIZE 128
#define SEMAPHORE_NAME_1 "/my_semaphore_1"
#define SEMAPHORE_NAME_2 "/my_semaphore_2"
#define POSIX_SHM_NAME "/shm_name_posix"

//для работы с сигналом переменная изначально имеющая положительное значение
volatile sig_atomic_t signal_work = 1;

//количество обработанных наборов
size_t num_naborov = 0;

typedef struct Share_memory //храним всю основную информацию о нашей разделяемой памяти
{
	int shmem_id_;
	int buf_[BUF_SIZE]; //массив в который записывать будем все элементы
	size_t this_; //будем отслеживать сколько записывает каждый процесс: this номер элемента с которого начал запись нынешний процесс
	size_t next_; //next номер элемента, до которого записал значения нынешний процесс
} Share_memory;

//для работы с сигналом, функция изменяющая сигнал на завершение работы
void sigint_handler(int signo) {
    signal_work = 0;
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

//функция чтения для родителя
void parent_read (Share_memory *sh_mem);
//функция записи для родителя
void parent_write (Share_memory *sh_mem);
//функция чтения для потомка
void descendant_read_write (Share_memory *sh_mem);
 int main (int argc, char * argv[])
{
	//указатель на разделяемую память
	Share_memory *sh_mem;
	
	//придётся использовать временные переменные так как памяти изначально нет и некуда записать информацию 
	int shmem_id_temp; //временный ID для sh_mem
	
	//переменные для создания семафоров
	sem_t *sem_1;
	sem_t *sem_2;
	
	//для работы с процессами
	pid_t pid;
	
	//для rand и лучшей генерации рамномных чисел
	srand(time(NULL));
	
	//если с таким именем была уже удалим её тогда, ошибку обраьотаем но завершать работу программы не будем тк это не проблема в данном моменте
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
	//начальные значения индексов в массиве разделяемой памяти
	sh_mem->this_ = 0;
	sh_mem->next_= 0;
	
	//удалим семафор 1 и сеамфор 2 если такие были семафоры unlink 
	sem_unlink(SEMAPHORE_NAME_1);
	sem_unlink(SEMAPHORE_NAME_2);
	
	//создадим новые семафоры с начальными значениями 1  (количество процессов)
	if ( (sem_1 = sem_open(SEMAPHORE_NAME_1, O_CREAT, 0777, 1)) == SEM_FAILED ) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
	if ( (sem_2 = sem_open(SEMAPHORE_NAME_2, O_CREAT, 0777, 1)) == SEM_FAILED ) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
	
	
	//Изначально аблокировали семафор_1 чтобы потомок не смог обогнать родителя в выполнении операций
	sem_lock(sem_1); //дальше родитель будет писать пока потомок ждёт
	
	int val;
	//получаем значение семафора
	sem_getvalue(sem_1, &val);
	printf("Изначальное значение семафора: %d\n", val );
	signal(SIGINT, sigint_handler);
	
	if ( (pid = fork()) < 0 )
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	while ( signal_work ) //signal_work
	{
		//отслеживаем максимальный размер разделяемой памяти
		if ( sh_mem -> this_ >= BUF_SIZE ||  sh_mem -> next_ >= BUF_SIZE )
		{
			printf("Проблема с индексами или достигнут конец разделяемой памяти\n");
			break;
		}
		if (pid == 0) //потомок
		{
			sem_lock(sem_2);
			sem_lock(sem_1);
			
			descendant_read_write(sh_mem);
			
			sem_unlock(sem_2);
			sem_unlock(sem_1);
		}
		else //родитель
		{
			parent_write(sh_mem);
			sem_unlock(sem_1);
			
			
			sem_lock(sem_2);
			sem_lock(sem_1);
			parent_read(sh_mem);
			sem_unlock(sem_2);
		}
		printf("\n");
		//sleep(1);
	}
	
	//потомок завершает свою работу 
	if ( pid == 0 ) exit(EXIT_SUCCESS);
	
	// родитель дожидается потомка
	if (pid > 0) {
		int st;
		waitpid(pid, &st, 0);
	}
	
	//разблокирование семафора_1 родителем так как после последней итерации родителя он остаётся заблокированным
	sem_unlock(sem_1); //дальше родитель будет писать пока потомок ждёт
	
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
	
	// закрываем семафоры
	if ( sem_close(sem_1) == -1 )
	{
		perror("sem_close");
		exit(EXIT_FAILURE);
	}
	if ( sem_close(sem_2) == -1 )
	{
		perror("sem_close");
		exit(EXIT_FAILURE);
	}
	
	// unlink удаляем семафоры
	if( sem_unlink(SEMAPHORE_NAME_1) == -1)
	{
		perror("sem_unlink");
		exit(EXIT_FAILURE);
	}
	if( sem_unlink(SEMAPHORE_NAME_2) == -1)
	{
		perror("sem_unlink");
		exit(EXIT_FAILURE);
	}
	
	printf("Количество обработанных наборов: %ld\n", num_naborov);
	return 0;
}

//функция для чтения родителем
void parent_read (Share_memory *sh_mem)
{
	//проверяем вощможные ошибки
	if (sh_mem -> this_ == sh_mem -> next_)
	{
		printf("Родителю читать нечего ещё пока\n");
		return;
	}
	if (sh_mem -> this_ > sh_mem -> next_)
	{
		printf("Родитель нашёл беду с индексами this>next\n");
		return;
	}
	int max = 0, min = 0;
	//идём по нашему массиву в разделяемой памяти пока не дойдём до элемента 
	for (; sh_mem -> this_ <= sh_mem -> next_; ++(sh_mem -> this_) )
	{
		if (max == 0) max = sh_mem -> buf_ [sh_mem -> this_];
		else min = sh_mem -> buf_ [sh_mem -> this_] ;
	}
	
	//отчистим наш массив чтобы можно было использовать повторно
	memset(sh_mem -> buf_, 0, sizeof(int)*BUF_SIZE);
	//обнулим значения так как отчистили память 
	sh_mem->this_ = 0;
	sh_mem->next_= 0;
	
	printf("Родитель Max: %d Min: %d\n", max, min);
	
	++num_naborov;//увеличиваем количество обработанных наборов
}
//функция для записи родителем
void parent_write (Share_memory *sh_mem)
{
	//проверяем вощможные ошибки
	if (sh_mem -> this_ != sh_mem -> next_)
	{
		printf("Родитель нашёл ошибку - индексы сейчас должны быть равны\n");
		return;
	}
	if (sh_mem -> this_ > sh_mem -> next_)
	{
		printf("Родитель нашёл беду с индексами this>next\n");
		return;
	}
	//сколько будем писать элементов
	int num = rand() % (BUF_SIZE - 4) + 2; // можем записать максимум 128 элементов - 2 элемента которые будут выделены для возвращения потомком наименьшего и наибольшего значения ( -4 так как потом прибавим 2, прибавим 2 так как наим число элементов хочу два)
	
	//если в итоге получится больше чем есть памяти тогда округлим до размеров памяти - хотя не должно
	if ( (num + sh_mem -> next_) >= BUF_SIZE ) 
	{
		//используем только sh_mem -> next_ (не используем num) так как можно обойтись без него дальше
		sh_mem -> next_ = BUF_SIZE - 1;
	}
	else sh_mem -> next_ += num; //иначе просто прибавляем количество чисел 
	
	//тк мы работает с индексами у нас отсчёт идёт с 0 поэтому конечное значение -1
	--(sh_mem -> next_);
	
	//проходимся в цикле до sh_mem -> next_ и заполняем непредвиденными числами
	printf("Родитель rand: ");
	for ( size_t i = sh_mem -> this_; i <= sh_mem -> next_; ++i)
	{
		sh_mem -> buf_ [i] = rand () % 100;
		printf ("%d ", sh_mem -> buf_ [i]);
	}
	printf ("\n");

}
//функция чтения и записи для потомка
void descendant_read_write (Share_memory *sh_mem)
{
	//проверяем вощможные ошибки
	if (sh_mem -> this_ == sh_mem -> next_)
	{
		printf("Потомку читать нечего ещё пока\n");
		return;
	}
	if (sh_mem -> this_ > sh_mem -> next_)
	{
		printf("Потмок нашёл беду с индексами this>next\n");
		return;
	}
	int max = INT_MIN, min = INT_MAX;
	//идём по нашему массиву в разделяемой памяти пока не дойдём до элемента next_ и ищем наибольший и наименьший элементы
	for (size_t i = sh_mem -> this_; i <= sh_mem -> next_; ++i )
	{
		if ( max < (sh_mem -> buf_ [i]) ) max = sh_mem -> buf_ [i];
		if ( min > (sh_mem -> buf_ [i]) ) min = sh_mem -> buf_ [i];
	}
	//приравниваем индексы
	sh_mem -> this_ = sh_mem -> next_;
	//пишем знаения масимального и минимального элементов
	sh_mem -> buf_ [sh_mem -> this_ + 1] = max;
	sh_mem -> buf_ [sh_mem -> this_ + 2] = min;
	
	
	//пишем правильные значения индексов next_ ушёл на 2 элемента, а так как в цикле мы итерируем первый цикл с 0 (не инкрементируя ) поставим this на новый элемент в памяти
	++(sh_mem -> this_);
	sh_mem -> next_ += 2;
	
	printf("Потомок Max: %d Min: %d\n", max, min);
}