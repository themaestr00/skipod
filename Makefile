CC=gcc
FLAGS=-fopenmp -$(OPTIMIZE)
DIR=$(PWD)/build

all: var714 var714_parallel var714_parallel_task

var714: $(PWD)/src/var714_$(N)_$(OPTIMIZE).c
	$(CC) $(FLAGS) -o $(DIR)/$@ $<

var714_parallel: $(PWD)/src/var714_parallel_$(N)_$(OPTIMIZE).c
	$(CC) $(FLAGS) -o $(DIR)/$@ $<

var714_parallel_task: $(PWD)/src/var714_parallel_task_$(N)_$(OPTIMIZE).c
	$(CC) $(FLAGS) -o $(DIR)/$@ $<

clean:
	rm -f $(DIR)/*
