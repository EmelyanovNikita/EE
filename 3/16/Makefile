FLAGS ?= -O2


all: server_main.o client_main.o
	gcc $(FLAGS) server_main.o -o server_exe.x
	gcc $(FLAGS) client_main.o -o client_exe.x
	
	
server_main.o: ./source/server/main.c
	gcc $(FLAGS) -c ./source/server/main.c -o server_main.o -lrt
	
	
client_main.o: ./source/client/main.c
	gcc $(FLAGS) -c ./source/client/main.c -o client_main.o -lrt
	
	
	
clean:
	rm -rf *.o *.x 