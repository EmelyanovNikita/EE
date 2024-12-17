

all: main.c libcalc.so
	gcc main.c -o calc.x -ldl

libcalc.so: calc.c
	gcc -fPIC  calc.c -c 
	gcc -shared calc.o -o libcalc.so -lm

clean:
	rm *.x *.o *.so