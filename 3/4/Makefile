FLAGS ?= -O2


all: main.o 
	gcc $(FLAGS) main.o -o exe.x  #TBook.o 
	
	
	
# TBook.o: ./source/TelephoneBook.c
	# gcc $(FLAGS) -I./include -c ./source/TelephoneBook.c -o TBook.o -w
	
main.o: ./source/main.c
	gcc $(FLAGS) -c ./source/main.c -o main.o 
	

clean:
	rm -rf *.x *.o *.log *.txt *.1