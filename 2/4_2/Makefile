FLAGS ?= -O2


all: ./source/main.c MyQueue.o
	gcc  $(FLAGS) -I./include ./source/main.c MyQueue.o -o mylist.x -w
	
	
MyQueue.o: ./source/MyQueue.c
	gcc  $(FLAGS) -c -I./include ./source/MyQueue.c -o MyQueue.o
	
	
clean:
	rm -rf *.o *.x 
