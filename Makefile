CC=gcc
FLAGS=-fopenmp -$(OPTIMIZE)
DIR=$(PWD)/build

all: var714 var714_parallel var714_parallel_task

var714: $(PWD)/src/var714.c
	$(CC) $(FLAGS) -o $(DIR)/$@_$(OPTIMIZE) $<

var714_parallel: $(PWD)/src/var714_parallel.c
	$(CC) $(FLAGS) -o $(DIR)/$@_$(OPTIMIZE) $<

var714_parallel_task: $(PWD)/src/var714_parallel_task.c
	$(CC) $(FLAGS) -o $(DIR)/$@_$(OPTIMIZE) $<

clean:
	rm -f $(DIR)/*
