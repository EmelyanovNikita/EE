#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>



int sum(int num, ...)
{
	int result = 0;
    va_list factor;         //указатель va_list
    va_start(factor, num);    // устанавливаем указатель
    for(int i=0;i<num; i++)
    {
        result += va_arg(factor, int);  // получаем значение текущего параметра типа int
	}
    va_end(factor); // завершаем обработку параметров
    return result;
}


int main(int argc, char *argv[])
{
	int result = 0;
	
	//printf("argc: %d : %d %d\n", argc, atoi(argv[1]), atoi(argv[2]));
	switch(argc)
		{
			case 1: 
				printf("Не может быть передано менее 1 аргумента\n");
			break;
				
			case 2:
				result = sum(argc - 1, atoi(argv[1]));
			break;
			
			case 3:
				result = sum(argc - 1, atoi(argv[1]), atoi(argv[2]));
			break;
			
			case 4:
				result = sum(argc - 1, atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
			break;
			
			case 5:
				result = sum(argc - 1, atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
			break;
			
			case 6:
				result = sum(argc - 1, atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]),  atoi(argv[5]));
			break;
			
			case 7:
				result = sum(argc - 1, atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]),  atoi(argv[5]), atoi(argv[6]));
			break;
			
			case 8:
				result = sum(argc - 1, atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]),  atoi(argv[5]), atoi(argv[6]), atoi(argv[7]) );
			break;
			
			case 9:
				result = sum(argc - 1, atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]),  atoi(argv[5]), atoi(argv[6]), atoi(argv[7]),  atoi(argv[8]));
			break;
			
			case 10:
				result = sum(argc - 1, atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]),  atoi(argv[5]), atoi(argv[6]), atoi(argv[7]),  atoi(argv[8]),   atoi(argv[9]));
			break;
			default:
				printf("Количество переменных от 1 до 10\n");
			break;
			
			
			
		}
		
		
		if (argc >= 2) printf("%d\n", result);
		
		
}