//
// min-heap priority queue implementation as described in Yandex School of Data Analysis
// book "Introduction to the theory of Algorithms and Data Structures"
//
// TODO:
//    change element priority
//    remove element from the heap

#include <math.h>

#include <mystd.h>

// external functions
#include <min_heap.h>

// internal functions
void __heap_sift_down     (heap *, int);
void __heap_sift_up       (heap *, int);
void __heap_swap_nodes    (heap *, int, int);

// given the heap and some key index in it preserves (and fixes when needed)
// the heap property down to the bottom (leaf) of the heap
void __heap_sift_down(heap *h, int j)
{
    // first child of the given key
    int i = j * 2;

    // both children exist and one of them is smaller than given key (parent)
    while (i + 1 <= h->last_used && (h->heap[i/2] > h->heap[i] || h->heap[i/2] > h->heap[i+1])) {

        // swap parent with the smallest of its children and continue further (down) 
        if (h->heap[i] < h->heap[i+1]) {
            __heap_swap_nodes(h, i/2, i);
            i = i * 2;
        } else {
            __heap_swap_nodes(h, i/2, i+1);
            i = (i + 1) * 2;
        }
    }

    // only one child, at the end
    if (i <= h->last_used && h->heap[i/2] > h->heap[i]) {
        // swap parent with this only child if the child is smaller than the parent
        __heap_swap_nodes(h, i/2, i);
    }
}

// extracts minimum key from the heap into *k (if it exists)
// and shuffles the tree so that heap property holds
//
// returns NULL (and doesn't modify *k) if there are no keys in the heap
//
// if there are keys, then
//    copies minimum key from heap to the variable pointed to by k
//    sets associated_struct pointer to the struct associated with the k
//
void *heap_extract_min(heap *h, HEAP_KEY_TYPE *k, void **associated_struct)
{
    if (h->last_used == 0) {
        return NULL;
    }

    // this should be something like copy_key(k, &h->heap[1])
    *k = h->heap[1];
    *associated_struct = h->heap_struct[1];

    if (h->last_used == 1) {
        h->last_used = 0;
    }
    else {
        __heap_swap_nodes (h, 1, h->last_used);
        h->last_used--;
        __heap_sift_down(h, 1);
    }

    return k;
}

// i and j are valid nodes, exchange their values
void __heap_swap_nodes (heap *h, int i, int j)
{
    if (i < 1 || i > HEAP_SIZE) {
        die("Programmer error: __heap_swap_nodes got invalid key index (1st)");
    }
    if (j < 1 || j > HEAP_SIZE) {
        die("Programmer error: __heap_swap_nodes got invalid key index (2nd)");
    }

    HEAP_KEY_TYPE tmp = h->heap[i];
    void *tmp_struct = h->heap_struct[i];

    h->heap[i] = h->heap[j];
    h->heap_struct[i] = h->heap_struct[j];

    h->heap[j] = tmp;
    h->heap_struct[j] = tmp_struct;
}

// given binary heap and its key preserves (and fixes when needed)
// the heap property up to the top (root) of the heap
//
void __heap_sift_up(heap *h, int i)
{
    // * 1 is the root of the tree
    //
    // * "in C, as in many other languages, integer division truncates"
    //   so i/2 effectively equals to (int) floor(i/2)
    //
    // * h->heap[i/2] > h->heap[i] is the condition which breaks min-heap property
    //
    while (i >= 2 && h->heap[i/2] > h->heap[i]) {

        // swap i/2 with i
        __heap_swap_nodes(h, i/2, i);

        // move on (upper) to the parent of i
        i = i/2;
    }
}

void heap_insert(heap *h, HEAP_KEY_TYPE key, void *associated_struct)
{
    if (h->last_used >= HEAP_SIZE) {
        die("No empty space in heap, unable to insert element");
    }

    h->last_used++;
    h->heap[h->last_used] = key;
    h->heap_struct[h->last_used] = associated_struct;
    __heap_sift_up(h, h->last_used);
}

void heap_create(heap *h, char *name)
{
    if (strlen(name) < 1) {
        die("Programmer error: need heap name to create it");
    }
    else if (strlen(name) > STRUCT_NAME_LENGTH) {
        die("Programmer error: heap name must be no longer than %d", STRUCT_NAME_LENGTH);
    }

    strcpy(h->name, name);
    
    int i;
    for (i = 1; i <= HEAP_SIZE; i++) {
        h->heap[i] = 0;
        h->heap_struct[i] = NULL;
    }

    h->last_used = 0;
}

bool heap_is_empty (heap *h)
{
    return h->last_used == 0;
}

bool heap_can_insert (heap *h)
{
    return h->last_used < HEAP_SIZE;
}

// slow, time to traverse whole tree is O(N)
//
// 1) calculate height of the heap
// 2) for every level (from 1 to height)
//    print all the nodes corresponding to the given level
//
void heap_dump_structured(heap *h)
{
    printf("\nDumping heap [%s]:\n", h->name);
    
    if (heap_is_empty(h)) {
        printf("empty heap.\n\n");
        return;
    }

    int heap_height = (int) floor(log2(h->last_used));
//    printf("heap_height: %d\n", heap_height);

    int j;
    for (j = 0; j <= heap_height; j++) {
        int shift = (int) pow(2, heap_height - j) * 3;
        int step = (int) (pow(2, heap_height - j + 1) - 1) * 3;

//        printf("shift %d, step %d, ", shift, step);
        printf("h  %d:", j);
        
        printf("%*s", shift, " ");

        int current_level = pow(2, j);
        int next_level = pow(2, j + 1);

        while(current_level < next_level && current_level <= h->last_used) {
            printf("%*d", 3, h->heap[current_level]);
            printf("%*s", step, " ");
            current_level++;
        }
        printf("\n");
    }
}

void heap_dump_storage(heap *h)
{
    int i;

    printf("\nDumping heap storage [%s], last_used [%d]:\n", h->name, h->last_used);
    for (i = 1; i <= h->last_used; i++) {
        printf("%d ", h->heap[i]);
    }
    printf("\n");

    printf("\nDumping heap_struct storage [%s], last_used [%d]:\n", h->name, h->last_used);
    for (i = 1; i <= h->last_used; i++) {
        if (h->heap_struct[i] == NULL) {
            printf("NULL ");
        }
        else {
            printf("%p ", h->heap_struct[i]);
        }
    }
    printf("\n");
}

boolean heap_equal(heap *h1, heap *h2)
{
    if (h1->last_used != h2->last_used) {
        return false;
    }

    int j;
    for (j = 1; j <= h1->last_used; j++) {
        if (h1->heap[j] != h2->heap[j]) {
            return false;
        }
    }

    return true;
}

boolean heap_test()
{
    int test2[HEAP_SIZE + 1] = { 0, -2, 1, -1, };

    heap mheap;
    heap_create(&mheap, "min_heap 1");
    heap_insert(&mheap, 1, NULL);
    heap_insert(&mheap, -1, NULL);
    heap_insert(&mheap, -2, NULL);
    int j;
    for (j = 1; j <= 3; j++) {
        if (test2[j] != mheap.heap[j]) {
            printf("test 1.%d failed, got %d instead of %d\n", j+1, mheap.heap[j], test2[j]);
            exit(1);
        }
        else {
            printf("test 1.%d passed\n", j + 1);
        }
    }

    heap test3 = { "test2", {0, 0, 1, 2, 3, }, {}, 4};

    heap mheap2;
    heap_create(&mheap2, "min_heap 2");
    heap_insert(&mheap2, 3, NULL);
    heap_insert(&mheap2, 2, NULL);
    heap_insert(&mheap2, 1, NULL);
    heap_insert(&mheap2, 0, NULL);

    if (heap_equal(&mheap2, &test3) != true) {
        printf("test 3 failed:\n");
        heap_dump_storage(&mheap2);
        heap_dump_storage(&test3);
    }
    else {
        printf("test 2 (reverse order) passed\n");
    }

    heap test4 = { "test3", {0, 1, 3, 2, }, {}, 3};

    HEAP_KEY_TYPE key;
    void *key_struct = NULL;
    
    if (heap_extract_min(&mheap2, &key, &key_struct) == NULL) {
        printf("test 3 failed: expected to get min key, but heap is empty\n");
    }
    else if (heap_equal(&mheap2, &test4) != true) {
        printf("test 3 failed: heaps are not equal\n");
        heap_dump_storage(&mheap2);
        heap_dump_storage(&test4);
    }
    else {
        printf("test 3 (extract min key) passed\n");
    }

    return true;
}
