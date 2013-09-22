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

#define MAX_HASH_SIZE 100
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

void hash_dump (struct HASH *h) {
    printf("Dumping hash [%s]: ", h->name);
    
    int num_keys = 0;
    int i;

    for (i=0; i < MAX_HASH_SIZE; i++) {
        if (h->used[i] == TRUE) {
            num_keys++;
            printf("\n  position [%d] used by key [%d]", i, h->keys[i]);
        }
    }

    if (num_keys == 0) {
        printf("empty hash\n");
    }
    else {
        printf("\n  = %d keys\n", num_keys);
    }
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

        i++;
        if (i == MAX_HASH_SIZE) {
            i = 0;
        }
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

            i++;
            if (i == MAX_HASH_SIZE) {
                i = 0;
            }

            break;
        }

        i++;
        if (i == MAX_HASH_SIZE) {
            i = 0;
        }
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

        i++;
        if (i == MAX_HASH_SIZE) {
            i = 0;
        }
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

        i++;
        if (i == MAX_HASH_SIZE) {
            i = 0;
        }
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

    hash_remove(&h1, 24);
    hash_remove(&h1, 14);
    hash_remove(&h1, 24);
    hash_add(&h1, 24);
    hash_dump(&h1);

    int j;
    for (j=1; j<101; j++) {
        hash_add(&h1, rand() % 1000);
    }

    hash_dump(&h1);

    return 0;
}
