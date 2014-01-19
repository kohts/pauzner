#define HEAP_SIZE 1000
#define HEAP_KEY_TYPE int

typedef struct __type_heap heap;

// forward/interface declaration
HEAP_KEY_TYPE * heap_extract_min (heap *, HEAP_KEY_TYPE *);
void heap_dump_structured (heap *);
void heap_dump_storage    (heap *);
void heap_insert          (heap *, HEAP_KEY_TYPE);
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
    
    // points to the first free leaf in the tree
    int last_used;
} heap;

