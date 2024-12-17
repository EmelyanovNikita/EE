#include <stdio.h>
#include <math.h>
#include <stdarg.h>


int div(int num, ...)
{
	int result = 0;
	double temp;
    va_list factor;         //указатель va_list
    va_start(factor, num);    // устанавливаем указатель
    for(int i=0;i<num; i++)
    {
		if(i==0) 
		{
			result = va_arg(factor, int);
			continue;
		}
		temp = (double)va_arg(factor, int);
		if(temp==0) 
		{
			continue;
		}
		result = (int)round((double)result / temp);
    }
    va_end(factor); // завершаем обработку параметров
    return result;
}
