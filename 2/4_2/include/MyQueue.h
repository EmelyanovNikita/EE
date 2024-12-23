#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#define MAXQUEUE 30


typedef struct Node
{
	int priority_;
	int data_;
} node;


typedef struct MyQueue{
	node Queue_[MAXQUEUE];
	int size_;
} MyQueue;


void init_values(MyQueue * queue);
bool is_full(MyQueue * queue);
bool is_empty(MyQueue * queue);
void add_element (MyQueue * queue, int priority, int data);
void delete_element(MyQueue * queue, int priority, bool variant);
void balance_queue(MyQueue * queue);
void print_queue(MyQueue * queue);












// int pop_first_node(MyQueue* queue, int priority);
