FLAGS ?= -O2


all: main.o
	gcc $(FLAGS) main.o -o main.x
	
	
main.o: ./source/main.c
	gcc $(FLAGS) -c ./source/main.c -o main.o -w
	
	
	
clean:
	rm -rf *.o *.x 