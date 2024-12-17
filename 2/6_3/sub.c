#include <stdio.h>
#include <math.h>
#include <stdarg.h>


int sub(int num, ...)
{
	int result = 0;
    va_list factor;         //указатель va_list
    va_start(factor, num);    // устанавливаем указатель
    for(int i=0;i<num; i++)
    {
		if(i==0) 
		{
			result = va_arg(factor, int);
			continue;
		}
        result -= va_arg(factor, int);  // получаем значение текущего параметра типа int
    }
    va_end(factor); // завершаем обработку параметров
    return result;
}