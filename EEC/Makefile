FLAGS ?= -O2


all: ./cliem.c Product_list.o lib_json_db.o cJSON.o QueueRcv.o client.o
	gcc  $(FLAGS) -I./ ./cliem.c Product_list.o lib_json_db.o cJSON.o QueueRcv.o client.o -o client.x
	
	
Product_list.o: ./Product_list.c
	gcc  $(FLAGS) -c -I./ ./Product_list.c -o Product_list.o
	
lib_json_db.o: ./lib_json_db.c 
	gcc  $(FLAGS) -c -I./ ./lib_json_db.c -o lib_json_db.o

cJSON.o: ./cJSON.c
	gcc  $(FLAGS) -c -I./ ./cJSON.c -o cJSON.o

QueueRcv.o: ./QueueRcv.c
	gcc  $(FLAGS) -c -I./ ./QueueRcv.c -o QueueRcv.o
	
client.o: ./client.c
	gcc  $(FLAGS) -c -I./ ./client.c -o client.o
	
clean:
	rm -rf *.o *.x 