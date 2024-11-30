FLAGS ?= -O2


all: main.o 
	gcc $(FLAGS) main.o -o exe.x  #TBook.o 
	
	
main.o: ./source/main.c
	gcc $(FLAGS) -c ./source/main.c -o main.o
	

clean:
	rm -rf *.x *.o *.1 *.txt