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

    int i;
    for (i = 0; i <= s->max_size; i++) {
        s->stack[i] = NULL;
    }

    s->top = 0;
}

void stack_free (stack *s) {
    if (s->max_size == 0) {
        die("Programmer error: stack_free called for uninitialized stack");
    }

    if (s->stack != NULL) {
        free(s->stack);
    }
    s->name[0] = 0;
    s->max_size = 0;
}
