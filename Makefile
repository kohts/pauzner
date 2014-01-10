INCLUDE=/home/petya/pauzner/github-pauzner

hash: hash.c
	gcc -Wall hash.c -o ../hash && ../hash
btree: btree.c
	gcc -Wall btree.c -lm -o ../btree && ../btree
min_heap: min_heap.c min_heap.h
	gcc -I $(INCLUDE) -Wall min_heap.c -lm -o ../min_heap && ../min_heap
sort_million_files: sort_million_files.c min_heap.c
	gcc -I $(INCLUDE) -Wall sort_million_files.c -lm -o ../sort_million_files
