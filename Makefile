CC=gcc
FLAGS=-fopenmp $(OPTIMIZE)
DIR=$(PWD)/build

all: var714 var714_parallel var714_parallel_task

var714: src/var714.c
	$(CC) $(FLAGS) -o $(DIR)/$@ $<

var714_parallel: src/var714_parallel.c
	$(CC) $(FLAGS) -o $(DIR)/$@ $<

var714_parallel_task: src/var714_parallel_task.c
	$(CC) $(FLAGS) -o $(DIR)/$@ $<

clean:
	rm -f $(DIR)/var714 $(DIR)/var714_parallel $(DIR)/var714_parallel_task
