FLAGS ?= -O2


all: main.o TBook.o
	gcc $(FLAGS) -I./include main.o TBook.o -o tbook.x	-w
	
	
	
TBook.o: ./source/TelephoneBook.c
	gcc $(FLAGS) -I./include -c ./source/TelephoneBook.c -o TBook.o -w
	
main.o: ./source/main.c
	gcc $(FLAGS)  -I./include -c ./source/main.c -o main.o -w
	

clean:
	rm -rf *.x *.o *.log *.txt