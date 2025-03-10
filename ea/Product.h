#ifndef PRODUCT_H
#define PRODUCT_H

#include <stdint.h>  


#define MAXLINE 30 /* максимальный размер вводимой строки */
#define MAXTEXT 150 /* максимальный размер вводимого текста */



typedef struct Product {
	char name_[MAXLINE]; 
	float cost_;
	size_t quantity_;
	char description_[MAXTEXT];
}Product;


#endif // PRODUCT_H