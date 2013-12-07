// hashmap implementation as described in Shen's book
// "Programming: theorems and excercises"
//
// colliding keys are stored in the same array as other keys,
// they are not grouped into linked list array (buckets)
//

#include "mystd.h"

#define MIN_FIXED_FIELD_WIDTH 3
#define MAX_HASH_SIZE 10
#define HASH_KEY_TYPE int

struct HASH {
    char name[STRUCT_NAME_LENGTH];
    bool used[MAX_HASH_SIZE];
    HASH_KEY_TYPE keys[MAX_HASH_SIZE];
};

int hash_function (HASH_KEY_TYPE key) {
    return key % MAX_HASH_SIZE;
}

int next_by_clock (int position) {
    int new_position;

    new_position = position + 1;
    if (new_position == MAX_HASH_SIZE) {
        new_position = 0;
    }

    return new_position;
}

void hash_dump (struct HASH *h) {
    int i;

    int longest_key = MIN_FIXED_FIELD_WIDTH;
    int current_key;
    char *tmp_str;
    tmp_str = malloc(MAX_MSG_SIZE);

    sprintf(tmp_str, "%d", (int) MAX_HASH_SIZE - 1);

    current_key = strlen(tmp_str);
    if (longest_key < current_key) {
        longest_key = current_key;
    }

    // find longest key
    for (i=0; i < MAX_HASH_SIZE; i++) {
        if (h->used[i] == TRUE) {
            sprintf(tmp_str, "%d", (int) h->keys[i]);
            current_key = strlen(tmp_str);

            if (longest_key < current_key) {
                longest_key = current_key;
            }
        }
    }
    free(tmp_str);

    printf("\nDumping hash table [%s]:\n", h->name);
    printf("    pos: ");
    for (i=0; i < MAX_HASH_SIZE; i++)
        printf("%0*d ", longest_key, i);
    printf("\n");

    printf("    u[]: ");
    for (i=0; i < MAX_HASH_SIZE; i++) {
        if (h->used[i] == TRUE) {
            printf("%*d ", longest_key, TRUE);
        }
        else {
            printf("%*d ", longest_key, FALSE);
        }
    }
    printf("\n");

    printf("    k[]: ");
    for (i=0; i < MAX_HASH_SIZE; i++) {
        if (h->used[i] == TRUE) {
            printf("%*d ", longest_key, h->keys[i]);
        }
        else {
            printf("%-*s ", longest_key, " ");
        }
    }
    printf("\n\n");
}

void hash_init (struct HASH *h, char name[]) {
    if (strlen(name) < 1) {
        die("hash_init: need hash name");
    }
    else if (strlen(name) > STRUCT_NAME_LENGTH) {
        char *msg;
        msg = malloc(MAX_MSG_SIZE);
        sprintf(msg, "hash_init: hash name must be no longer than %d", STRUCT_NAME_LENGTH);
        die(msg);
        free(msg);
    }

    strcpy(h->name, name);

    int i;
    for(i=0; i < MAX_HASH_SIZE; i++) {
        h->used[i] = FALSE;
        h->keys[i] = 0;
    }
}

bool hash_add (struct HASH *h, HASH_KEY_TYPE key) {
    int kh = hash_function(key);
    int i = kh;

    while (h->used[i] == TRUE) {
        if (h->keys[i] == key) {
            return TRUE;
        }

        i = next_by_clock(i);

        if (i == kh) {
            if (DEBUG == TRUE) {
                char *msg;
                msg = malloc(MAX_MSG_SIZE);
                sprintf(msg, "[%s] hash_add: unable to add key [%d], no space left in hash", h->name, key);
                die(msg);
                free(msg);
            }

            return FALSE;
        }
    }

    h->used[i] = TRUE;
    h->keys[i] = key;
    
    if (DEBUG == TRUE) {
        char *msg;
        msg = malloc(MAX_MSG_SIZE);
        sprintf(msg, "[%s] hash_add: added key [%d] (key hash: %d, key position: %d)", h->name, key, kh, i);
        debug(msg);
        free(msg);
    }

    return TRUE;
}

bool hash_remove (struct HASH *h, HASH_KEY_TYPE key) {
    int kh = hash_function(key);
    int i = kh;
    bool found_key;
    int hole;

    while (h->used[i] == TRUE) {
        if (h->keys[i] == key) {
            found_key = TRUE;

            hole = i;
            h->used[hole] = FALSE;
            h->keys[hole] = 0;

            i = next_by_clock(i);

            break;
        }

        i = next_by_clock(i);

        if (i == kh) {
            return FALSE;
        }
    }

    if (found_key == FALSE) {
        return FALSE;
    }

    while (h->used[i] == TRUE) {
        if (hash_function(h->keys[i]) == i) {
        }
        else if (
            (i - hash_function(h->keys[i]) + MAX_HASH_SIZE) % MAX_HASH_SIZE >=
            (i - hole + MAX_HASH_SIZE) % MAX_HASH_SIZE
            ) {
            h->used[hole] = TRUE;
            h->keys[hole] = h->keys[i];

            hole = i;
            h->used[hole] = FALSE;
            h->keys[hole] = 0;
        }

        i = next_by_clock(i);

        if (i == kh) {
            return TRUE;
        }
    }

    return TRUE;
}

bool hash_exists (struct HASH *h, HASH_KEY_TYPE key) {
    int kh = hash_function(key);
    int i = kh;

    while (h->used[i] == TRUE) {
        if (h->keys[i] == key) {
            return TRUE;
        }

        i = next_by_clock(i);

        if (i == kh) {
            return FALSE;
        }
    }

    return FALSE;
}

bool hash_equal (struct HASH *h1, struct HASH *h2) {
    int i = 0;

    while (i < MAX_HASH_SIZE) {
        if (h1->used[i] == h2->used[i]) {
            if (h1->used[i] == TRUE) {
                if (h1->keys[i] != h2->keys[i]) {
                    return FALSE;
                }
            }
        }
        else {
            return FALSE;
        }
        i++;
    }

    return TRUE;
}

// unit tests for hash
bool hash_test() {
    struct HASH h1;
    struct HASH h2;

    hash_init(&h1, "actual");
    hash_init(&h2, "expected");

    hash_add(&h1, 9);
    hash_add(&h1, 19);
    hash_remove(&h1, 9);

    hash_add(&h2, 19);

    if (hash_equal(&h1, &h2) != TRUE) {
        printf("failed!\n");
        hash_dump(&h2);
        hash_dump(&h1);
        return FALSE;
    }

    hash_init(&h1, "actual");
    hash_init(&h2, "expected");

    hash_add(&h1, 3);
    hash_add(&h1, 4);
    hash_add(&h1, 13);
    hash_add(&h1, 5);
    hash_add(&h1, 14);
    hash_remove(&h1, 3);

    hash_add(&h2, 13);
    hash_add(&h2, 4);
    hash_add(&h2, 5);
    hash_add(&h2, 14);

    if (hash_equal(&h1, &h2) != TRUE) {
        printf("failed!\n");
        hash_dump(&h2);
        hash_dump(&h1);
        return FALSE;
    }


    hash_init(&h1, "actual");
    hash_init(&h2, "expected");

    hash_add(&h1, 3);
    hash_add(&h1, 4);
    hash_add(&h1, 5);
    hash_add(&h1, 6);
    hash_add(&h1, 44);
    hash_add(&h1, 8);
    hash_add(&h1, 33);
    hash_remove(&h1, 4);

    hash_add(&h2, 3);
    hash_add(&h2, 5);
    hash_add(&h2, 6);
    hash_add(&h2, 44);
    hash_add(&h2, 8);
    hash_add(&h2, 33);

    if (hash_equal(&h1, &h2) != TRUE) {
        printf("failed!\n");
        hash_dump(&h2);
        hash_dump(&h1);
        return FALSE;
    }

    return TRUE;
}

int main(int argc, char *argv[]) {
    struct HASH h1;

    if (argc == 2 && strcmp("test", argv[1]) == 0) {
        if (hash_test() == TRUE) {
            printf("tests passed\n");
            return 0;
        }
        else {
            return 1;
        }
    }

    if (argc > 1) DEBUG=TRUE;

    char cmd[MAX_MSG_SIZE] = "";
    char cmd_short[MAX_MSG_SIZE] = "";
    int key;

    hash_init(&h1, "test");
    hash_dump(&h1);

    while (readcmd("[a]dd N, [r]emove N, [p]rint, [q]uit: ", cmd)) {
//        printf("got [%s]\n", cmd);
        if (strcmp(cmd, "q") == 0) {
            printf("bye\n");
            break;
        }

        if (strstr(cmd, "a") == &cmd[0] || strstr(cmd, "r") == &cmd[0]) {
            if (sscanf(cmd, "%s %d", cmd_short, &key) != 2) {
                printf("invalid command: %s\n", cmd);
                continue;
            }
//        printf("got: [%s] [%d]\n", cmd_short, key);
            printf("\n");

            if (strcmp(cmd_short, "a") == 0) {
                hash_add(&h1, key);
            }
            if (strcmp(cmd_short, "r") == 0) {
                hash_remove(&h1, key);
            }
        }

        hash_dump(&h1);
    }

    return 0;
}
