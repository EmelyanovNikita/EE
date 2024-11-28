#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>



void str_sum(int num, char * result, ...)
{
	//char result[150];
    va_list factor;         //указатель va_list
    va_start(factor, result);    // устанавливаем указатель
    for(int i=0; i < num; i++)
    {
		if( i == 0 ) strcpy(result,  va_arg(factor, char*));
        else strcat(result, va_arg(factor, char*));  // получаем значение текущего параметра типа char
	}
    va_end(factor); // завершаем обработку параметров
   // return result;
}


int main(int argc, char *argv[])
{
	char result[150];
	
	
	switch(argc)
		{
			case 1: 
				printf("Не может быть передано менее 1 аргумента\n");
			break;
				
			case 2:
				str_sum(argc - 1, result, argv[1]);
			break;
			
			case 3:
				str_sum(argc - 1, result, argv[1], argv[2]);
			break;
			
			case 4:
				str_sum(argc - 1, result, argv[1], argv[2], argv[3]);
			break;
			
			case 5:
				str_sum(argc - 1, result,  argv[1],  argv[2],  argv[3],  argv[4] );
			break;
			
			case 6:
				str_sum(argc - 1, result, argv[1],  argv[2],  argv[3],  argv[4], argv[5] );
			break;
			
			case 7:
				str_sum(argc - 1, result,argv[1],  argv[2],  argv[3],  argv[4], argv[5], argv[6]);
			break;
			
			case 8:
				str_sum(argc - 1, result, argv[1],  argv[2],  argv[3],  argv[4], argv[5], argv[6],  argv[7] );
			break;
			
			case 9:
				str_sum(argc - 1, result, argv[1],  argv[2],  argv[3],  argv[4], argv[5], argv[6],  argv[7], argv[8]);
			break;
			
			case 10:
				str_sum(argc - 1, result, argv[1],  argv[2],  argv[3],  argv[4], argv[5], argv[6],  argv[7], argv[8], argv[9] );
			break;
			case 11:
				str_sum(argc - 1, result, argv[1],  argv[2],  argv[3],  argv[4], argv[5], argv[6],  argv[7], argv[8], argv[9],  argv[10]);
			break;
			default:
				printf("Количество переменных от 1 до 11\n");
			break;
			
			
			
		}
		
		
		if (argc >= 2) printf("%s\n", result);
		
		
}