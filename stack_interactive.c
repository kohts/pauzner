//
// stack interactive tool
//

#include <mystd.h>
#include <stack.h>

int main(int argc, char *argv[]) {
    if (argc == 2 && strcmp("test", argv[1]) == 0) {
        if (stack_test() == true) {
            printf("all tests are ok.\n");
        }

        return 0;
    }

    stack s;
    stack_create(&s, "stack 1", 100);

    stack_dump(&s);

    stack_free(&s);

    return 0;
}
