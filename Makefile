hash: hash.c
	gcc -Wall hash.c -o ../hash && ../hash
btree: btree.c
	gcc -Wall btree.c -lm -o ../btree && ../btree
min_heap: min_heap.c
	gcc -Wall min_heap.c -lm -o ../min_heap && ../min_heap
