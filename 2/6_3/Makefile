

all: main.c libsum.so libsub.so libdiv.so
	gcc main.c -o calc -ldl

libsum.so: sum.c
	gcc -fPIC  sum.c -c 
	gcc -shared sum.o -o libsum.so -lm

libsub.so: sub.c
	gcc -fPIC  sub.c -c 
	gcc -shared sub.o -o libsub.so -lm
	
libdiv.so: div.c
	gcc -fPIC  div.c -c 
	gcc -shared div.o -o libdiv.so -lm
	

clean:
	rm *.x *.o *.so calc