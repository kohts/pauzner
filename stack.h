typedef struct __type_stack stack;

// stack
typedef struct __type_stack {
    char name[STRUCT_NAME_LENGTH];
    
    void **stack; // 1..top

    int top; // points to the current top of the stack
    
    // maximum size of the stack, number of elements it can hold,
    // set in constructor
    int max_size;
} stack;

void stack_create          (stack *, char *, int);
void stack_free            ();
bool stack_test            ();
void stack_dump            (stack *);
