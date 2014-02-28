INCLUDE=/home/petya/pauzner/github-pauzner
CC_PARAM=-Wall -I $(INCLUDE)

mystd.o: mystd.c mystd.h
	gcc $(CC_PARAM) -c mystd.c

hash: hash.c mystd.o
	gcc $(CC_PARAM) -o ../hash hash.c mystd.o
	../hash

btree: btree.c mystd.o
	gcc $(CC_PARAM) -o ../btree btree.c mystd.o -lm
	../btree

min_heap.o: min_heap.c min_heap.h
	gcc $(CC_PARAM) -c min_heap.c
min_heap_interactive: min_heap_interactive.c min_heap.o mystd.o
	gcc $(CC_PARAM) -o ../min_heap_interactive min_heap_interactive.c min_heap.o mystd.o -lm
	../min_heap_interactive

merge_million_files: merge_million_files.c mystd.o min_heap.o
	gcc $(CC_PARAM) -o ../merge_million_files merge_million_files.c mystd.o min_heap.o -lm

stack.o: stack.c stack.h mystd.o
	gcc $(CC_PARAM) -c stack.c
stack_interactive: stack_interactive.c stack.o
	gcc $(CC_PARAM) -o ../stack_interactive stack_interactive.c stack.o mystd.o

shunting_yard: stack.o
	gcc $(CC_PARAM) -o ../shunting_yard shunting_yard.c stack.o mystd.o -lm

.PHONY : clean
clean:
	rm -f *.o
