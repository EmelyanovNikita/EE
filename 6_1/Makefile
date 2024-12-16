FLAGS ?= -O2

COMMONINC = -I./include

override FLAGS += $(COMMONINC)


all: ./source/main.c TelBook.a
	gcc $(FLAGS) ./source/main.c -o mylist.x -L . TelBook.a 
	

TelBook.a: TelBook.o
	ar r TelBook.a TelBook.o
	
	
TelBook.o: ./source/TelBook.c
	gcc  $(FLAGS) -c -I./include ./source/TelBook.c -o TelBook.o
	
	
clean:
	rm -rf *.o *.x  *.a