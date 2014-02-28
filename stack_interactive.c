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

    char cmd[MAX_MSG_SIZE] = "";
    char cmd_short[MAX_MSG_SIZE] = "";
    char stack_arg[MAX_MSG_SIZE] = "";
    char *tmp;

    while (readcmd("[a]dd TEXT, [pop], [top], [d]ump stack storage, [q]uit: ", cmd)) {
        if (strcmp(cmd, "q") == 0) {
            printf("bye\n");
            break;
        }
        if (strcmp(cmd, "d") == 0) {
            stack_dump_storage(&s);
        }
        if (strcmp(cmd, "pop") == 0) {
            if (stack_is_empty(&s) != true) {
                tmp = stack_pop(&s);
                printf("got element from stack: %s", tmp);
            } else {
                printf("empty stack, unable to get element");
            }
        }
        if (strcmp(cmd, "top") == 0) {
            tmp = stack_top(&s);
            printf("top of the stack is: %s", tmp);
        }

        if (
            strstr(cmd, "a") == &cmd[0]
            ) {
            if (sscanf(cmd, "%s %s", cmd_short, stack_arg) != 2) {
                printf("invalid command: %s\n", cmd);
                continue;
            }
//        printf("got: [%s] [%d]\n", cmd_short, key);
            printf("\n");
            if (strcmp(cmd_short, "a") == 0) {
                stack_push(&s, stack_arg);
            }
        }

        stack_dump(&s);
    }

    stack_free(&s);

    return 0;
}
