#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MSG_SIZE 1024

typedef short int bool;
#define TRUE 1
#define FALSE 0

bool DEBUG = FALSE;

void die (char msg[]) {
    printf("fatal error: %s\n", msg);
    exit(1);
}
void debug(char msg[]) {
    printf("debug: %s\n", msg);
}

#define MIN_FIXED_FIELD_WIDTH 3
#define MAX_HASH_SIZE 10
#define HASH_KEY_TYPE int
#define MAX_HASH_NAME_LENGTH 100

struct HASH {
    char name[MAX_HASH_NAME_LENGTH];
    bool used[MAX_HASH_SIZE];
    HASH_KEY_TYPE keys[MAX_HASH_SIZE];
};

int hash_function (HASH_KEY_TYPE key) {
    return key % MAX_HASH_SIZE;
}

int collision_function(int position) {
    int new_position;

    new_position = position + 1;
    if (new_position == MAX_HASH_SIZE) {
        new_position = 0;
    }

    return new_position;
}

void hash_dump (struct HASH *h) {
    int num_keys = 0;
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

    printf("Dumping hash table [%s]:\n", h->name);
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
    else if (strlen(name) > MAX_HASH_NAME_LENGTH) {
        char *msg;
        msg = malloc(MAX_MSG_SIZE);
        sprintf(msg, "hash_init: hash name must be no longer than %d", MAX_HASH_NAME_LENGTH);
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

        i = collision_function(i);

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
    
            if (DEBUG == TRUE) {
                char *msg;
                msg = malloc(MAX_MSG_SIZE);
                sprintf(msg, "[%s] hash_remove: removed key [%d]", h->name, key);
                debug(msg);
                free(msg);
            }

            i = collision_function(i);

            break;
        }

        i = collision_function(i);

        if (i == kh) {
            if (DEBUG == TRUE) {
                char *msg;
                msg = malloc(MAX_MSG_SIZE);
                sprintf(msg, "[%s] hash_remove: unable to remove key [%d], it's not in hash", h->name, key);
                debug(msg);
                free(msg);
            }

            return FALSE;
        }
    }

    if (found_key == FALSE) {
        if (DEBUG == TRUE) {
            char *msg;
            msg = malloc(MAX_MSG_SIZE);
            sprintf(msg, "[%s] hash_remove: unable to remove key [%d], it's not in hash", h->name, key);
            debug(msg);
            free(msg);
        }

        return FALSE;
    }

    while (h->used[i] == TRUE) {
        if (hash_function(h->keys[i]) == hole) {
            h->used[hole] = TRUE;
            h->keys[hole] = h->keys[i];

            hole = i;
            h->used[hole] = FALSE;
            h->keys[hole] = 0;
        }

        i = collision_function(i);

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

        i = collision_function(i);

        if (i == kh) {
            return FALSE;
        }
    }

    return FALSE;
}

int main(int argc, char *argv[]) {
    struct HASH h1;
    bool res;

    if (argc > 1) DEBUG=TRUE;

    hash_init(&h1, "test");
    hash_dump(&h1);

    if (!hash_add(&h1, 14))
        die("error adding key");
    if (!hash_add(&h1, 15))
        die("error adding key");
    if (!hash_add(&h1, 16))
        die("error adding key");
    hash_dump(&h1);

    if (!hash_add(&h1, 13))
        die("error adding key");
    if (!hash_add(&h1, 23))
        die("error adding key");
    hash_dump(&h1);

    hash_remove(&h1, 13);
    hash_dump(&h1);

    if (!hash_add(&h1, 99))
        die("error adding key");
    if (!hash_add(&h1, 199))
        die("error adding key");
    hash_dump(&h1);

    if (!hash_add(&h1, 98))
        die("error adding key");
    if (!hash_add(&h1, 198))
        die("error adding key");
    hash_dump(&h1);

    if (!hash_add(&h1, 7))
        die("error adding key");
    hash_dump(&h1);

    hash_remove(&h1, 98);
    hash_dump(&h1);

//    int j;
//    for (j=1; j<101; j++) {
//        hash_add(&h1, rand() % 1000);
//    }
//    hash_dump(&h1);

    return 0;
}
