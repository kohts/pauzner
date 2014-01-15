//
// min-heap priority queue implementation as described
// in Yandex School of Data Analysis "Algorithms and Data Structures" book
//
// TODO:
//    search for the element (one element can be present several times in the heap)
//    change element(s) priority
//    remove element(s) from the heap


// max number of elements in heap
#define HEAP_SIZE 1000
#define HEAP_KEY_TYPE int

// min-heap priority queue
typedef struct __type_min_heap {
    char name[STRUCT_NAME_LENGTH];
    
    HEAP_KEY_TYPE heap[HEAP_SIZE+1];  // 1..last_used
    int last_used;

} heap;

// forward declarations
void heap_create     (heap *);
bool heap_is_empty   (heap *);
bool heap_can_insert (heap *);
void heap_insert     (heap *, HEAP_KEY_TYPE);
HEAP_KEY_TYPE heap_exptract_min (heap *);

// internal functions
void sift_up         (heap *);
void sift_down       (heap *);


void heap_create (heap *h)
{
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

void heap_insert (heap *h, HEAP_KEY_TYPE k)
{
    if (h->last_used < HEAP_SIZE) {
        h->last_used++;
        h->heap[h->last_used] = k;
        sift_up(h);
    }
    // should report on else case (no space)?? or ignore silently?
}

HEAP_KEY_TYPE heap_extract_min (heap *h)
{
    if (h->last_used < 1) {
       exit(1234); // fatal error!!!
    }

    HEAP_KEY_TYPE ret = h->heap[1];
    h->heap[1] = h->heap[h->last_used];
    h->last_used--;
    sift_down(h);
    return ret;
}

// i and j are valid nodes, exchange their values
void __heap_swap_nodes (heap *h, int i, int j)
{
    HEAP_KEY_TYPE tmp = h->heap[i];
    h->heap[i] = h->heap[j];
    h->heap[j] = tmp;
}

// walk from last_used to 1: swap i with parent (i/2) while parent is larger
void sift_up (heap *h)
{
    int i = h->last_used;

    while (i => 2 && h->heap[i/2] > h->heap[i]) {
        // swap i/2 with i
        __heap_swap_nodes(h, i/2, i);
        // decrement i
        i = i/2;
    }
}

// walk from 1 to last_used: swap i with the smallest child (2*i or 2*i+1) while parent is larger
void sift_down (heap *h)
{
    int i = 2;

    // A: both child exists
    while (i + 1 <= h->last_used && (h->heap[i/2] > h->heap[i] || h->heap[i/2] > h->heap[i+1])) {
        if (h->heap[i] < h->heap[i+1]) {
            //swap i with i/2
            __heap_swap_nodes(h, i/2, i);
        } else {
            //swap i+1 with i/2
            __heap_swap_nodes(h, i/2, i+1);
        }
        // increment i
        i = i*2;
    }

    // B: only one child, at the end
    if (i <= h->last_used && h->heap[i/2] > h->heap[i]) {
        // swap i with i/2
        __heap_swap_nodes(h, i/2, i);
    }
}

