INCLUDE=/home/petya/pauzner/github-pauzner
CC_PARAM=-Wall -I $(INCLUDE)

mystd.o: mystd.c mystd.h
	gcc $(CC_PARAM) -c mystd.c

hash: hash.c mystd.h mystd.o
	gcc $(CC_PARAM) -o ../hash hash.c mystd.o
	../hash

btree: btree.c mystd.h mystd.o
	gcc $(CC_PARAM) -o ../btree btree.c mystd.o -lm
	../btree

min_heap.o: min_heap.c min_heap.h
	gcc -c $(CC_PARAM) min_heap.c
min_heap: min_heap.c mystd.h mystd.o
	gcc $(CC_PARAM) -o ../min_heap min_heap.c mystd.o -lm
	../min_heap

merge_million_files: merge_million_files.c mystd.h mystd.o
	gcc $(CC_PARAM) -o ../merge_million_files merge_million_files.c mystd.o


.PHONY : clean
clean:
	rm *.o
