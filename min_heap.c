//
// min-heap priority queue implementation as described
// in Yandex School of Data Analysis "Algorithms and Data Structures" book
//
// TODO:
//    search for the element (one element can be present several times in the heap)
//    change element(s) priority
//    remove element(s) from the heap

#include "mystd.h"
#include <math.h>

// max number of elements in heap
#define HEAP_SIZE 1000
#define HEAP_KEY_TYPE int

// min-heap priority queue
typedef struct __type_min_heap {
    char name[STRUCT_NAME_LENGTH];
    
    // heap storage, zeroed in min_heap_create
    HEAP_KEY_TYPE heap[HEAP_SIZE];
    
    // points to the first free leaf in the tree
    int heap_pointer;

} min_heap;

// forward declarations
void min_heap_dump(min_heap *);
void dump_heap_storage(min_heap *);
void sift_down(min_heap *, int);
void sift_up(min_heap *, int);

// given 0-based pointer of the element in the heap array
// returns 0-based pointer to the parent of this element
//
int min_heap_parent_pointer (int heap_pointer) {
    if (heap_pointer == 0) {
        die("Programmer error: requested to find parent for the root of the heap");
    }

    return floor((heap_pointer - 1) / 2);
}

// given 0-based pointer of the element in the heap array
// returns 1-based level of the element (root is at the height 1,
// first two children are at the height 2, and so on)
//
int min_heap_child_level (int heap_pointer) {
    if (heap_pointer == 0) {
        return 1;
    }
    else {
        return floor(log2(heap_pointer + 1)) + 1;
    }
}

// returns true if parent_key and child_key meet heap property
// and false otherwise
boolean min_heap_property_holds (HEAP_KEY_TYPE parent_key, HEAP_KEY_TYPE child_key) {
    if (parent_key <= child_key) {
        return true;
    }
    else {
        return false;
    }
}

// given the heap and its key preserves (and fixes when needed)
// the heap property down to the bottom (leaf) of the heap
void sift_down(min_heap *h, int pointer) {
    int children_base_pointer;
    int min_child_pointer;

    children_base_pointer = pointer * 2;

    // first possible child of the given key points to the free space in the heap;
    // current key is a leaf
    if (children_base_pointer + 1 >= h->heap_pointer) {
        return;
    }

    if (children_base_pointer + 2 < h->heap_pointer) {
        // there're two children defined for the current key,
        // choose the one which guarantees the heap property
        // if chosen to replace the current key
        if (min_heap_property_holds(h->heap[children_base_pointer + 1], h->heap[children_base_pointer + 2]) == true) {
            min_child_pointer = children_base_pointer + 1;
        }
        else {
            min_child_pointer = children_base_pointer + 2;
        }
    }
    else if (children_base_pointer + 1 < h->heap_pointer) {
        // there's only one child for the current key
        min_child_pointer = children_base_pointer + 1;
    }

    // if current key doesn't break the heap property with the chosen
    // "minimal" child, then we don't need to sift it down further
    if (min_heap_property_holds(h->heap[pointer], h->heap[min_child_pointer]) == true) {
        return;
    }

    // exchange "minimal" child with the current key and continue sift-down
    HEAP_KEY_TYPE tmp;
    tmp = h->heap[pointer];
    h->heap[pointer] = h->heap[min_child_pointer];
    h->heap[min_child_pointer] = tmp;

//    dump_heap_storage(h);
//    printf("%d\n", min_child_pointer);

    return sift_down(h, min_child_pointer);
}

// extracts minimum key from the heap into *k (if it exists)
// and shuffles the tree so that heap property holds
//
// returns NULL (and doesn't modify *k) if there are no keys in the heap
//
HEAP_KEY_TYPE *min_heap_extract_min(min_heap *h, HEAP_KEY_TYPE *k) {
    if (h->heap_pointer == 0) {
        return NULL;
    }

    // this should be something like copy_key(k, &h->heap[0])
    *k = h->heap[0];

    // empty heap
    if (h->heap_pointer == 1) {
        h->heap[0] = 0;
        h->heap_pointer = 0;
    }
    else {
        h->heap[0] = h->heap[h->heap_pointer - 1];
        h->heap[h->heap_pointer - 1] = 0;
        h->heap_pointer = h->heap_pointer - 1;
        sift_down(h, 0);
    }

    return k;
}

// given binary heap and its key preserves (and fixes when needed)
// the heap property up to the top (root) of the heap
//
void sift_up(min_heap *h, int pointer) {
    if (pointer == 0) {
        return;
    }

    int parent_pointer = min_heap_parent_pointer(pointer);

    if (min_heap_property_holds(h->heap[parent_pointer], h->heap[pointer]) == true) {
        return;
    }

    HEAP_KEY_TYPE temp_key;

    // exchange child and parent if heap property doesn't hold
    temp_key = h->heap[parent_pointer];
    h->heap[parent_pointer] = h->heap[pointer];
    h->heap[pointer] = temp_key;

    return sift_up(h, parent_pointer);
}

void min_heap_insert(min_heap *h, HEAP_KEY_TYPE key) {
    if (h->heap_pointer >= HEAP_SIZE) {
        die("No empty space in heap, unable to insert element");
    }

    h->heap[h->heap_pointer] = key;
    h->heap_pointer = h->heap_pointer + 1;
    return sift_up(h, h->heap_pointer - 1);
}

void min_heap_create(min_heap *h, char *name) {
    if (strlen(name) < 1) {
        die("min_heap_create: need min_heap name");
    }
    else if (strlen(name) > STRUCT_NAME_LENGTH) {
        char *msg;
        msg = malloc(MAX_MSG_SIZE);
        sprintf(msg, "min_heap_create: min_heap name must be no longer than %d", STRUCT_NAME_LENGTH);
        die(msg);
        free(msg);
    }

    strcpy(h->name, name);
    
    int i;
    for (i=0; i < HEAP_SIZE - 1; i++) {
        h->heap[i] = 0;
    }

    h->heap_pointer = 0;
}

// goes all the way down starting with the child_pointer
// and outputs keys for the required_level (height) of the heap
//
int min_heap_print_nodes_by_level (min_heap *h, int required_level, int child_pointer) {
    int current_level = min_heap_child_level(child_pointer);

    if (required_level == current_level) {
        if (child_pointer >= h->heap_pointer) {
            return 0;
        }

        int heap_height = min_heap_child_level(h->heap_pointer - 1);

        if (heap_height == current_level) {
            printf("%*d ", 3, h->heap[child_pointer]);
        }
        else {
            printf("%*d ", (int) round(3 * (pow(2, heap_height) / pow (2, current_level))), h->heap[child_pointer]);
        }
        return 1;
    }

    int nodes_printed = 0;

    // pointer     0  1  2  3  4  5  6  7  8  9  10  11 12 13 14
    // left node   1  3  5  7  9 11 13
    // right node  2  4  6  8 10 12 14

    // left node
    nodes_printed = nodes_printed + min_heap_print_nodes_by_level(h, required_level, child_pointer * 2 + 1);
    // right node
    nodes_printed = nodes_printed + min_heap_print_nodes_by_level(h, required_level, child_pointer * 2 + 2);

    return nodes_printed;
}

// slow, time to traverse whole tree is O (log N * log N)
//
// 1) calculate height of the heap
// 2) for every level (from 1 to height)
//    print all the nodes corresponding to the given level
//
void min_heap_dump(min_heap *h) {
    printf("\nDumping min_heap [%s]:\n", h->name);
    
    if (h->heap_pointer == 0) {
        printf("empty min_heap.\n\n");
        return;
    }

    int heap_height = min_heap_child_level(h->heap_pointer - 1);
    //printf("heap pointer %d, height %d\n", h->heap_pointer, heap_height);

    int j;
    for (j = 1; j <= heap_height; j++) {
        printf("h %d: ", j);
        min_heap_print_nodes_by_level(h, j, 0);
        printf("\n");
    }

}

void dump_heap_storage(min_heap *h) {
    printf("\nDumping heap storage [%s], heap_pointer [%d]:\n", h->name, h->heap_pointer);

    int i;
    for (i=0; i < h->heap_pointer; i++) {
        printf("%d ", h->heap[i]);
    }
    printf("\n");
}

boolean min_heap_equal(min_heap *h1, min_heap *h2) {
    if (h1->heap_pointer != h2->heap_pointer) {
        return false;
    }

    int j;
    for (j = 0; j < h1->heap_pointer - 1; j++) {
        if (h1->heap[j] != h2->heap[j]) {
            return false;
        }
    }

    return true;
}

boolean min_heap_test() {
    //                       1  2  3  4  5  6  7  8  9 10  11  12  13  14  15  16
    //                       0  1  2  3  4  5  6  7  8  9  10  11  12  13  14  15
    //                      
    int test1[HEAP_SIZE] = { 1, 2, 2, 3, 3, 3, 3, 4, 4, 4,  4,  4,  4,  4,  4,  5, };
    
    //
    // 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
    //         +---------V--V
    // +-V-V +-|-----V-V      
    // 0 1 2 3 4 5 6 7 8 9  10 11 12 13 14 15
    //   +-|-^-^              
    //     +-----^-^           
    //
    int i;
    for (i = 0; i <= 15; i++) {
        if (min_heap_child_level(i) != test1[i]) {
            printf("test 1.%d failed, got %d instead of %d", i+1, min_heap_child_level(0), test1[i]);
            exit(1);
        }
        else {
            printf("test 1.%d passed\n", i + 1);
        }
    }

    int test2[HEAP_SIZE] = { -2, 1, -1, };

    min_heap mheap;
    min_heap_create(&mheap, "min_heap 1");
    min_heap_insert(&mheap, 1);
    min_heap_insert(&mheap, -1);
    min_heap_insert(&mheap, -2);
    int j;
    for (j = 0; j < 3; j++) {
        if (test2[j] != mheap.heap[j]) {
            printf("test 2.%d failed, got %d instead of %d\n", j+1, mheap.heap[j], test2[j]);
            exit(1);
        }
        else {
            printf("test 2.%d passed\n", j + 1);
        }
    }


    min_heap test3 = { "test3", {0, 1, 2, 3, }, 4};
//    min_heap_create(&test3, "test3");
//   test3.heap = {0, 1, 2, 3};
//   test3.heap_pointer = 4;

    min_heap mheap2;
    min_heap_create(&mheap2, "min_heap 2");
    min_heap_insert(&mheap2, 3);
    min_heap_insert(&mheap2, 2);
    min_heap_insert(&mheap2, 1);
    min_heap_insert(&mheap2, 0);

    if (min_heap_equal(&mheap2, &test3) != true) {
        printf("test 3 failed:\n");
        dump_heap_storage(&mheap2);
        dump_heap_storage(&test3);
    }
    else {
        printf("test 3 (reverse order) passed\n");
    }

    min_heap test4 = { "test4", {1, 3, 2, }, 3};
    HEAP_KEY_TYPE key;
    if (min_heap_extract_min(&mheap2, &key) == NULL) {
        printf("test 4 failed: expected to get min key, but heap is empty\n");
    }
    else if (min_heap_equal(&mheap2, &test4) != true) {
        printf("test 4 failed: heaps are not equal\n");
        dump_heap_storage(&mheap2);
        dump_heap_storage(&test4);
    }
    else {
        printf("test 4 (extract min key) passed\n");
    }

    return true;
}

int main(int argc, char *argv[]) {
    if (argc == 2 && strcmp("test", argv[1]) == 0) {
        if (min_heap_test() == true) {
            printf("all tests are ok.\n");
        }

        return 0;
    }

    min_heap mheap;
    min_heap_create(&mheap, "min_heap 1");
    min_heap_dump(&mheap);

    char cmd[MAX_MSG_SIZE] = "";
    char cmd_short[MAX_MSG_SIZE] = "";
    HEAP_KEY_TYPE key;

    while (readcmd("[a]dd N, [d]ump heap storage, [e]xtract min, [q]uit: ", cmd)) {
//        printf("got [%s]\n", cmd);
        if (strcmp(cmd, "q") == 0) {
            printf("bye\n");
            break;
        }
        if (strcmp(cmd, "d") == 0) {
            dump_heap_storage(&mheap);
        }
        if (strcmp(cmd, "e") == 0) {
            if (min_heap_extract_min(&mheap, &key) == NULL) {
                printf("empty heap, unable to extract min key\n");
            }
            else {
                printf("got min key: %d\n", key);
            }
        }

        if (
            strstr(cmd, "a") == &cmd[0]
            ) {
            if (sscanf(cmd, "%s %d", cmd_short, &key) != 2) {
                printf("invalid command: %s\n", cmd);
                continue;
            }
//        printf("got: [%s] [%d]\n", cmd_short, key);
            printf("\n");
            if (strcmp(cmd_short, "a") == 0) {
                min_heap_insert(&mheap, key);
            }
        }

        min_heap_dump(&mheap);
    }

    return 0;
}
