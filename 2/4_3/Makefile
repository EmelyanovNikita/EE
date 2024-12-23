FLAGS ?= -O2


all: ./source/main.c TelBook.o
	gcc  $(FLAGS) -I./include ./source/main.c TelBook.o -o mybst.x -w
	
	
TelBook.o: ./source/TelBook.c
	gcc  $(FLAGS) -c -I./include ./source/TelBook.c -o TelBook.o
	
	
clean:
	rm -rf *.o *.x 