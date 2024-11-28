FLAGS ?= -O2


all: my_shell str_sum sum diff


my_shell: ./source/main.c
	gcc $(FLAGS) ./source/main.c -o my_shell

str_sum: ./str_sum_dir/str_sum.c
	gcc $(FLAGS) ./str_sum_dir/str_sum.c -o str_sum 

sum: ./sum_dir/sum.c
	gcc $(FLAGS) ./sum_dir/sum.c -o sum

diff: ./diff_dir/diff.c
	gcc $(FLAGS) ./diff_dir/diff.c -o diff

clean:
	rm -rf *.x *.o my_shell diff sum str_sum