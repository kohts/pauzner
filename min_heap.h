// tests do not pass with such a huge HEAP_SIZE, this should be
// allocated dynamically, without using stack
#define HEAP_SIZE 200000

#define HEAP_KEY_TYPE int

typedef struct __type_heap heap;

// forward/interface declaration

// all three parameters are modified inside inside the call
// first one is the heap on which we operate,
// second is the storage for extracted key,
// third is the storage for the pointer to optional
// associated with the key structure
//
void *heap_extract_min    (heap *, HEAP_KEY_TYPE *, void **);

void heap_insert          (heap *, HEAP_KEY_TYPE, void *);
void heap_dump_structured (heap *);
void heap_dump_storage    (heap *);
void heap_create          (heap *, char *);
bool heap_equal           (heap *, heap *);
bool heap_is_empty        (heap *);
bool heap_can_insert      (heap *);
bool heap_test            ();

// min-heap priority queue
typedef struct __type_heap {
    char name[STRUCT_NAME_LENGTH];
    
    // heap storage, zeroed in heap_create
    //
    // ATTN: 0 element is not used (!)
    HEAP_KEY_TYPE heap[HEAP_SIZE + 1]; // 1..last_used

    // storage for pointer to the struct associated with the heap element
    void *heap_struct[HEAP_SIZE + 1];

    // points to the first free leaf in the tree
    int last_used;
} heap;

