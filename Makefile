hash: hash.c
	gcc -Wall hash.c -o ../hash && ../hash
btree: btree.c
	gcc -Wall btree.c -lm -o ../btree && ../btree
