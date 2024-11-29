FLAGS ?= -O2


all: client.x server.x


client.x: ./source/client.c
	gcc $(FLAGS) ./source/client.c -o client.x -lrt -w

server.x: ./source/server.c
	gcc $(FLAGS) ./source/server.c -o server.x -lrt -w

clean:
	rm -rf *.x
