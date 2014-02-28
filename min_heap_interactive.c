//
// min-heap priority queue interactive tool
//

#include <mystd.h>
#include <min_heap.h>

int main(int argc, char *argv[]) {
    if (argc == 2 && strcmp("test", argv[1]) == 0) {
        if (heap_test() == true) {
            printf("all tests are ok.\n");
        }

        return 0;
    }

    heap mheap;
    heap_create(&mheap, "min_heap 1", 100);

    heap_dump_structured(&mheap);

    char cmd[MAX_MSG_SIZE] = "";
    char cmd_short[MAX_MSG_SIZE] = "";
    HEAP_KEY_TYPE key;
    void *associated_struct = NULL;

    while (readcmd("[a]dd N, [d]ump heap storage, [e]xtract min, [q]uit: ", cmd)) {
//        printf("got [%s]\n", cmd);
        if (strcmp(cmd, "q") == 0) {
            printf("bye\n");
            break;
        }
        if (strcmp(cmd, "d") == 0) {
            heap_dump_storage(&mheap);
        }
        if (strcmp(cmd, "e") == 0) {
            if (heap_extract_min(&mheap, &key, &associated_struct) == NULL) {
                printf("empty heap, unable to extract min key\n");
            } else {
                printf("got min key: %d\n", key);
            }
        }

        if (
            strstr(cmd, "a") == &cmd[0]
            ) {
            if (sscanf(cmd, "%s %d", cmd_short, &key) != 2) {
                printf("invalid command: %s\n", cmd);
                continue;
            }
//        printf("got: [%s] [%d]\n", cmd_short, key);
            printf("\n");
            if (strcmp(cmd_short, "a") == 0) {
                heap_insert(&mheap, key, NULL);
            }
        }

        heap_dump_structured(&mheap);
    }

    heap_free(&mheap);

    return 0;
}
