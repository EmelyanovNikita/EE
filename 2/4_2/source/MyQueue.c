#include "MyQueue.h"


void init_values(MyQueue * queue)
{
	queue->size_ = 0;
	for(int i=0; i< MAXQUEUE; ++i)
	{
		queue->Queue_[i].priority_ = -1;
		queue->Queue_[i].data_ = -1;
	}
}


bool is_full(MyQueue * queue) 
{ 
	if (queue == NULL) return true;
	return queue->size_ == MAXQUEUE ? true : false;
}


bool is_empty(MyQueue * queue)
{
	if (queue == NULL) return true;
	return queue->size_ == 0 ? true : false;
}


void add_element (MyQueue * queue, int priority, int data)
{
	if (queue == NULL) return;
	if (is_full(queue)) 
	{
		printf("Элемент [%d](%d) не вставлен, очередь заполнена\n", priority, data);
		return;
	}
	
	
	if ( priority > 256 || priority < 0) return;
	
	
	node temp;
	bool marker = false;//маркер для определния надо ли сдвигать элементы массива или нет
	

	for(int i=0; i< MAXQUEUE; ++i)
	{
		if(marker == false)
		{
			
			if ( queue->size_ == 0 ) //если первый элемент тогда вставляем в самое начало очереди
			{
				queue->Queue_[i].priority_ = priority;
				queue->Queue_[i].data_ = data;
				++queue->size_;
				return;
			}
			else if ( i == 0 && queue->Queue_[i].priority_ > priority)
			{	//если у нашего элемента наименьший (0 - уйдёт самым первым) приоритет в очереди
				temp.priority_ = queue->Queue_[i].priority_;
				temp.data_ = queue->Queue_[i].data_;
				queue->Queue_[i].priority_ = priority;
				queue->Queue_[i].data_ = data;
				++queue->size_;
				marker = true;
			}
			else if ( queue->Queue_[i].priority_ == -1 && queue->Queue_[i-1].priority_ <= priority)
			{//если массив ещё заполнен изначальными данными тогда проверяем чтобы предыдущий элемент был просто меньше или равен вставляемому
				queue->Queue_[i].priority_ = priority;
				queue->Queue_[i].data_ = data;
				++queue->size_;
				return;
			}
			else if ( queue->Queue_[i-1].priority_ <= priority && queue->Queue_[i].priority_ > priority)
			{//классический вараинт - вставка между двух элементов один из которых меньше или равен другой больше
				temp.priority_ = queue->Queue_[i].priority_;
				temp.data_ = queue->Queue_[i].data_;
				queue->Queue_[i].priority_ = priority;
				queue->Queue_[i].data_ = data;
				++queue->size_;
				marker = true;
			}
		}
		else 
		{
			if( queue->Queue_[i].priority_ == -1 )
			{	//если элементы ещё заполенные наальными значениями тогда просто вставляем без дальнейшего сдвига 
				queue->Queue_[i].priority_ = temp.priority_;
				queue->Queue_[i].data_ = temp.data_;
				return;
			}
			else
			{
				node temp_2;
				
				
				temp_2.priority_ = queue->Queue_[i].priority_;
				temp_2.data_ = queue->Queue_[i].data_;
				
				queue->Queue_[i].priority_ = temp.priority_;
				queue->Queue_[i].data_ = temp.data_;
				
				temp.priority_ = temp_2.priority_;
				temp.data_ = temp_2.data_;
			}
		}
	}
	
}


void pop_front(MyQueue * queue)
{
	if ( is_empty(queue) ) 
	{
		printf("Очередь пуста, удалять нечего тут\n");
		return ;
	}
	
	for (int i=0; i < MAXQUEUE; ++i)
	{
		if ( i != MAXQUEUE-1 && queue->Queue_[i+1].priority_ == -1)
		{
			queue->Queue_[i].priority_ =  queue->Queue_[i+1].priority_;
			queue->Queue_[i].data_ =  queue->Queue_[i+1].data_;
			--queue->size_;
			return;
		}
		else if ( i == MAXQUEUE - 1 )
		{
			queue->Queue_[i].priority_ =  -1;
			queue->Queue_[i].data_ =  -1;
			--queue->size_;
			return;
		}
		else 
		{
			queue->Queue_[i].priority_ =  queue->Queue_[i+1].priority_;
			queue->Queue_[i].data_ =  queue->Queue_[i+1].data_;
		}
	}
}


void delete_element(MyQueue * queue, int priority, bool variant) //если variant == false тогда удаляем все с такми же приоритетом, иначе с с приоритетом не ниже чем
{
	if ( is_empty(queue) ) 
	{
		printf("Очередь пуста, удалять нечего тут\n");
		return ;
	}
	
	
	if ( priority > 256 || priority < 0)
	{
		printf("Неправильно заданные параметры приоритета 0<= приоритет <=256\n");
		return;
	}
	
	
	//если у первой ноды приоритет выше чем у целевой или если он равен -1 тогда return
	if( queue->Queue_[0].priority_ > priority || queue->Queue_[0].priority_ == -1) return;
	
	int temp_size = queue->size_; //запоминаем размер очереди чтобы пройтись по нему	
	for (int i = 0 ; i < temp_size;  ++i)
	{
		//удалим данные у всех у кого приоритет равен заданному
		if( queue->Queue_[i].priority_ == priority )
		{
			queue->Queue_[i].priority_ = -1;
			queue->Queue_[i].data_ = -1;
			--queue->size_;
		}
		//удалим данные у всеху кого приоритет ниже заданного приоритета
		else if( variant && queue->Queue_[i].priority_ < priority )
		{
			queue->Queue_[i].priority_ = -1;
			queue->Queue_[i].data_ = -1;
			--queue->size_;
		}
	}
	balance_queue(queue);
}


void balance_queue(MyQueue * queue) //мы получиди очередь с ужаленными внутри элментами (снаружи могут быть тоже но они нас не интересуют) 
{//и мы сейчас первым циклом посчитаем количество удалённых элементов и найдем элемент перед которым они стоят ( благо они идут подряд ) 
	// и затем вторым циклом переместим все элементы после найденного элемента на то количество элементов сколько их было удалено ранее
	int size_of_nulls = 0;
	bool temp = false;
	int target_index;
	
	
	for(int i = MAXQUEUE -1 ; i > -1; --i) //проходимся по массиву с конца ищем элементы ставшие -1 внутри заполненной части массива
	{
		if (queue->Queue_[i].priority_ != -1) temp = true;
		
		
		if (temp == true && queue->Queue_[i].priority_ == -1)
		{
			if(size_of_nulls == 0) target_index = i + 1; //только первый раз запоминаем индекс элемента перед которым начались удаленные элементы
			++size_of_nulls;
		}
	}
	if (size_of_nulls == 0 ) return;
	for(int i = target_index ; i < MAXQUEUE; ++i) //проходимся по массиву перенося элементы на size_of_nulls вперёд
	{
		queue->Queue_[i - size_of_nulls].priority_ = queue->Queue_[i].priority_;
		queue->Queue_[i - size_of_nulls].data_ = queue->Queue_[i].data_;
		
		queue->Queue_[i].priority_ = -1;
		queue->Queue_[i].data_ = -1;
	}
}


void print_queue(MyQueue * queue)
{
	if (queue == NULL) return;
	if ( is_empty(queue) ) 
	{
		printf("Очередь пуста\n");
		return ;
	}
	
	printf("Выводим [приоритеты] и (информацию)\n");
	for(int i = 0; i < queue->size_; ++i)
	{
		printf("[%d](%d)", queue->Queue_[i].priority_, queue->Queue_[i].data_);
		if (i != queue->size_-1) printf(" -> ");
	}
	
	
	printf("\n");
}