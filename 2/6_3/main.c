#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

#define MAXNAME 30
#define MAXFUNK 10
#define MAXVALUES 10


typedef int (*p_to_funk)(int num, ...);
typedef struct Funk_s{
	char name[MAXNAME];
	p_to_funk p_funk;
} Funk_s;


int menu()
{
	void *handle;
	int n, num, zapolnennye_funks = 0;
	p_to_funk div, sum , sub;
	
	//загрузка библиотеки
	handle = dlopen("./libcalc.so", RTLD_LAZY);
	if (!handle)
	{
		perror("dlopen");
		exit(EXIT_FAILURE);
	}
	
	//получение адресов функций
	div = (p_to_funk) dlsym(handle, "div");
	sum = (p_to_funk) dlsym(handle, "sum");
	sub = (p_to_funk) dlsym(handle, "sub");
	
	
	if( !sum || !div || !sub)
	{
		perror("dlsym");
		dlclose(handle);
		exit(EXIT_FAILURE);
	}
	
	Funk_s s_arr[MAXFUNK] = { {"сумма", sum},
	{"разность", sub} ,  {"деление", div}}; // ,  {"деление", div}
	
	int arr[MAXVALUES];
	int result;
	
	do {
		printf("Доступные функции:\n");
		for(int i=0; i < MAXFUNK; ++i)
		{
			if (s_arr[i].p_funk != 0) 
			{
				printf("%s - %d\n", s_arr[i].name,  i);
				zapolnennye_funks++;
			}
				
		}
		printf("Выйти - '-1'\n");
		scanf("%d", &n);
		
		if ( n==-1) break;
		while( (n < -1) || (n>=zapolnennye_funks)) 
		{
			printf("\nТакого варианта нет\nВыберите вариант из перечисленных выше: ");
			scanf("%d", &n);
		}
		
		printf("Количество переменных:\n");
		scanf("%d", &num);
		printf("Введите переменные:\n");
		
		switch(num)
		{
			case 1:
				for(int i=0; i<num; ++i)
				{
					scanf("%d", &arr[i]);
				}
				result = s_arr[n].p_funk(num, arr[0]);
			break;
				
			case 2:
				for(int i=0; i<num; ++i)
				{
					scanf("%d", &arr[i]);
				}
				result = s_arr[n].p_funk(num, arr[0], arr[1]);
			break;
			
			case 3:
				for(int i=0; i<num; ++i)
				{
					scanf("%d", &arr[i]);
				}
				result = s_arr[n].p_funk(num, arr[0], arr[1], arr[2]);
			break;
			
			case 4:
				for(int i=0; i<num; ++i)
				{
					scanf("%d", &arr[i]);
				}
				result = s_arr[n].p_funk(num, arr[0], arr[1], arr[2], arr[3]);
			break;
			
			case 5:
				for(int i=0; i<num; ++i)
				{
					scanf("%d", &arr[i]);
				}
				result = s_arr[n].p_funk(num, arr[0], arr[1], arr[2], arr[3], arr[4]);
			break;
			
			case 6:
				for(int i=0; i<num; ++i)
				{
					scanf("%d", &arr[i]);
				}
				result = s_arr[n].p_funk(num, arr[0], arr[1], arr[2], arr[3], arr[4], arr[5]);
			break;
			
			case 7:
				for(int i=0; i<num; ++i)
				{
					scanf("%d", &arr[i]);
				}
				result = s_arr[n].p_funk(num, arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6]);
			break;
			
			case 8:
				for(int i=0; i<num; ++i)
				{
					scanf("%d", &arr[i]);
				}
				result = s_arr[n].p_funk(num, arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6], arr[7]);
			break;
			
			case 9:
				for(int i=0; i<num; ++i)
				{
					scanf("%d", &arr[i]);
				}
				result = s_arr[n].p_funk(num, arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6], arr[7], arr[8]);
			break;
			
			case 10:
				for(int i=0; i<num; ++i)
				{
					scanf("%d", &arr[i]);
				}
				result = s_arr[n].p_funk(num, arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6], arr[7], arr[8], arr[9]);
			break;
			default:
				printf("Количество переменных от 1 до 10\n");
			break;
			
			
			
		}
		
		printf("Результат: %d\n", result);
		result = zapolnennye_funks = 0;
		
	} while(n!=-1);
	dlclose(handle);
	return 0;
}
int main ()
{
	
	menu();
	
	
	return 0;
}
