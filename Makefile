FLAGS ?= -O2

COMMONINC = -I./include

override FLAGS += $(COMMONINC)


all: ./source/main.c TelBook.lib
	gcc $(FLAGS) ./source/main.c -o mylist.x -L . TelBook.lib
	

TelBook.lib: TelBook.o
	ar r TelBook.lib TelBook.o
	
	
TelBook.o: ./source/TelBook.c
	gcc  $(FLAGS) -c -I./include ./source/TelBook.c -o TelBook.o
	
	
clean:
	rm -rf *.o *.x  *.lib