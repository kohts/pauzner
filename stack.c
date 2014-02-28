//
// stack implementation
//

#include <mystd.h>
#include <stack.h>

boolean stack_test()
{
    return true;
}

void stack_create(stack *s, char *name, int size)
{
    if (strlen(name) < 1) {
        die("Programmer error: need stack name to create it");
    } else if (strlen(name) > STRUCT_NAME_LENGTH) {
        die("Programmer error: stack name must be no longer than %d", STRUCT_NAME_LENGTH);
    }

    strcpy(s->name, name);

    s->max_size = size;

    s->stack = NULL;
    s->stack = (void **) malloc(sizeof(void *) * (s->max_size + 1));
    if (s->stack == NULL) {
        die("stack_create: unable to allocate %d bytes for stack storage, stack %s", sizeof(void *) * (s->max_size + 1), s->name);
    }

    s->char_storage_size = sizeof(char) * (s->max_size + 1) * STRUCT_NAME_LENGTH;
    s->char_storage = NULL;
    s->char_storage = (char *) malloc(s->char_storage_size);
    if (s->char_storage == NULL) {
        die("stack_create: unable to allocate %d bytes for stack char storage, stack %s", s->char_storage_size, s->name);
    }

    int i;
    int j;
    for (i = 0; i <= s->max_size; i++) {
        s->stack[i] = NULL;

        for (j = 0; j < STRUCT_NAME_LENGTH; j++) {
            s->char_storage[i+j] = 0;
        }
    }

    s->top = 0;
    s->next_free_char = 0;
}

void stack_free (stack *s) {
    if (s->max_size == 0) {
        die("Programmer error: stack_free called for uninitialized stack");
    }

    if (s->stack != NULL) {
        free(s->stack);
    }
    if (s->char_storage != NULL) {
        free(s->char_storage);
    }

    s->name[0] = 0;
    s->max_size = 0;
    s->top = 0;
    s->next_free_char = 0;
    s->char_storage_size = 0;
}

bool stack_push(stack *s, char *el)
{
    if (s->top >= s->max_size) {
        die("No empty space in the stack, unable to push element");
    }

    int el_size = strlen(el) + 1;
    if (el_size > STRUCT_NAME_LENGTH) {
        die("Programmer error: string [%s] is too large (%d bytes) for the stack [%s] element (max. element size is %d)", el, el_size, s->name, STRUCT_NAME_LENGTH);
    }
    if (s->next_free_char + el_size > s->char_storage_size) {
        die("Programmer error: no empty space in the char storage of stack [%s] for element [%s]", s->name, el);
    }

    s->top++;

    s->stack[s->top] = strcpy(&s->char_storage[s->next_free_char], el);
    s->next_free_char = s->next_free_char + el_size;

    return true;
}

char *stack_pop(stack *s)
{
    if (stack_is_empty(s)) {
        die("Programmer error: stack [%s] is empty, can't get element from it", s->name);
    }

    void *tmp = s->stack[s->top];
    
    s->stack[s->top] = NULL;
    s->top--;

    return (char *) tmp;
}

char *stack_top(stack *s)
{
    if (stack_is_empty(s)) {
        return NULL;
    }

    return (char *) s->stack[s->top];
}

bool stack_is_empty (stack *s)
{
    if (s->top < 1) {
        return true;
    } else {
        return false;
    }
}

void stack_dump(stack *s)
{
    printf("\nDumping stack [%s]: ", s->name);

    int i;
    for (i = 1; i <= s->max_size && s->stack[i] != NULL; i++) {
        printf("%d:%s ", i, (char *) s->stack[i]);
    }

    printf("\n\n");
}

void stack_dump_storage(stack *s)
{
    printf("\nDumping stack [%s] storage:\n", s->name);

    int current_el_start = 0;
    int current_el_number = 1;
    int i;

    for (i = current_el_start; i < s->next_free_char; i++) {
        if (s->char_storage[i] == '\0') {
            printf("element %d: %s\n", current_el_number, &s->char_storage[current_el_start]);
            current_el_start = i + 1;
            current_el_number++;
        }
    }

    printf("\n");
}
