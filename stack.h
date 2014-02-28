typedef struct __type_stack stack;

// stack
typedef struct __type_stack {
    char name[STRUCT_NAME_LENGTH];
    
    int char_storage_size;
    int next_free_char;
    char *char_storage; // max_size * STRUCT_NAME_LENGTH bytes

    void **stack; // 1..top

    int top; // points to the current top of the stack
    
    // maximum size of the stack, number of elements it can hold,
    // set in constructor
    int max_size;
} stack;

void stack_create          (stack *, char *, int);
void stack_free            ();
void stack_dump            (stack *);
void stack_dump_storage    (stack *);

bool stack_push            (stack *, char *);
bool stack_is_empty        (stack *);

char *stack_pop            (stack *);
char *stack_top            (stack *);

bool stack_test            ();
