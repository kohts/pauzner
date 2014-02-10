// hashmap implementation as described in Shen's book
// "Programming: theorems and excercises"
//
// colliding keys are stored in the same array as other keys,
// they are not grouped into linked list array (buckets)
//

#include "mystd.h"

#define MIN_FIXED_FIELD_WIDTH 3
#define HASH_KEY_TYPE int

typedef struct __type_hash {
    char name[STRUCT_NAME_LENGTH];
    bool *used;
    HASH_KEY_TYPE *keys;
    int size;
} hash;

int hash_function (HASH_KEY_TYPE key, int hash_size) {
    return key % hash_size;
}

int next_by_clock (int position, int hash_size) {
    int new_position;

    new_position = position + 1;
    if (new_position == hash_size) {
        new_position = 0;
    }

    return new_position;
}

void hash_dump (hash *h)
{
    int i;

    int longest_key = MIN_FIXED_FIELD_WIDTH;
    int current_key;
    char *tmp_str;
    tmp_str = malloc(MAX_MSG_SIZE);

    sprintf(tmp_str, "%d", (int) h->size - 1);

    current_key = strlen(tmp_str);
    if (longest_key < current_key) {
        longest_key = current_key;
    }

    // find longest key
    for (i=0; i < h->size; i++) {
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
    for (i=0; i < h->size; i++)
        printf("%0*d ", longest_key, i);
    printf("\n");

    printf("    u[]: ");
    for (i=0; i < h->size; i++) {
        if (h->used[i] == TRUE) {
            printf("%*d ", longest_key, TRUE);
        }
        else {
            printf("%*d ", longest_key, FALSE);
        }
    }
    printf("\n");

    printf("    k[]: ");
    for (i=0; i < h->size; i++) {
        if (h->used[i] == TRUE) {
            printf("%*d ", longest_key, h->keys[i]);
        }
        else {
            printf("%-*s ", longest_key, " ");
        }
    }
    printf("\n\n");
}

void hash_create (hash *h, char name[], int size)
{
    if (strlen(name) < 1) {
        die("hash_create: need hash name");
    } else if (strlen(name) > STRUCT_NAME_LENGTH) {
        die("hash_create: hash name must be no longer than %d", STRUCT_NAME_LENGTH);
    }

    strcpy(h->name, name);
    h->size = size;

    h->keys = NULL;
    h->keys = malloc(sizeof(HASH_KEY_TYPE) * h->size);
    if (h->keys == NULL) {
        die("hash_create: unable to allocate %d bytes for keys array, hash %s", sizeof(HASH_KEY_TYPE) * h->size, h->name);
    }

    h->used = NULL;
    h->used = malloc(sizeof(boolean) * h->size);
    if (h->used == NULL) {
        die("hash_create: unable to allocate %d bytes for used array, hash %s", sizeof(HASH_KEY_TYPE) * h->size, h->name);
    }

    int i;
    for(i=0; i < h->size; i++) {
        h->used[i] = FALSE;
        h->keys[i] = 0;
    }
}

void hash_free (hash *h) {
    if (h->size == 0) {
        die("Programmer error: hash_free called for uninitialized hash");
    }

    if (h->keys != NULL) {
        free(h->keys);
    }
    if (h->used != NULL) {
        free(h->used);
    }
    h->name[0] = 0;
    h->size = 0;
}

bool hash_add (hash *h, HASH_KEY_TYPE key)
{
    int kh = hash_function(key, h->size);
    int i = kh;

    // look for an empty place for the key starting with its own hash position
    // which might have already been occupied
    while (h->used[i] == TRUE) {
        
        // already existing key
        if (h->keys[i] == key) {
            return TRUE;
        }

        i = next_by_clock(i, h->size);

        // checked all the hash, no empty space for the key
        if (i == kh) {
            return FALSE;
        }
    }

    h->used[i] = TRUE;
    h->keys[i] = key;
    
    return TRUE;
}

bool hash_remove (hash *h, HASH_KEY_TYPE key)
{
    int kh = hash_function(key, h->size);
    int i = kh;
    bool found_key;
    int hole;

    // search for the requested key starting with its hash place
    // which might be occupied by some other key
    while (h->used[i] == TRUE) {

        // found the key 
        if (h->keys[i] == key) {
            found_key = TRUE;

            hole = i;
            h->used[hole] = FALSE;
            h->keys[hole] = 0;

            // i points to the first key after the created hole
            i = next_by_clock(i, h->size);

            break;
        }

        i = next_by_clock(i, h->size);

        // key is not present in hash at all, we've checked it all, can't remove
        if (i == kh) {
            return FALSE;
        }
    }

    if (found_key == FALSE) {
        return FALSE;
    }

    // possible move some keys to occupy the hole which was freed above
    while (h->used[i] == TRUE) {
        if (hash_function(h->keys[i], h->size) == i) {
            // if the key is in its place, skip it
        }
        else if (
            (i - hash_function(h->keys[i], h->size) + h->size) % h->size >=
            (i - hole + h->size) % h->size
            ) {
            // if the gap between the key's place and the place it should be (value of hash_function)
            // is greater than the gap between the key's place and the hole then "swap" the key with the hole
            
            h->used[hole] = TRUE;
            h->keys[hole] = h->keys[i];

            hole = i;
            h->used[hole] = FALSE;
            h->keys[hole] = 0;
        }

        i = next_by_clock(i, h->size);

        // all the keys are at their best possible places now
        // (we might have moved some of them before)
        if (i == kh) {
            return TRUE;
        }
    }
    
    return TRUE;
}

bool hash_exists (hash *h, HASH_KEY_TYPE key)
{
    int kh = hash_function(key, h->size);
    int i = kh;

    while (h->used[i] == TRUE) {
        if (h->keys[i] == key) {
            return TRUE;
        }

        i = next_by_clock(i, h->size);

        if (i == kh) {
            return FALSE;
        }
    }

    return FALSE;
}

bool hash_equal (hash *h1, hash *h2)
{
    int i = 0;

    int max_hash_size;
    if (h1->size > h2->size) {
        max_hash_size = h1->size;
    } else {
        max_hash_size = h2->size;
    }

    while (i < max_hash_size) {
        if (i < h1->size && i < h2->size) {
            if (h1->used[i] == h2->used[i]) {
                if (h1->used[i] == TRUE) {
                    if (h1->keys[i] != h2->keys[i]) {
                        return FALSE;
                    }
                }
            } else {
                return FALSE;
            }
        } else if (h1->size >= i) {
            if (h2->used[i] == TRUE) {
                return FALSE;
            }
        } else if (h2->size >= i) {
            if (h1->used[i] == TRUE) {
                return FALSE;
            }
        } else {
            die ("Programmer error: chosen max_hash_size %d for the hashes of size %d (%s) and %d (%s)",
                max_hash_size, h1->size, h1->name, h2->size, h2->name);
        }
        
        i++;
    }

    return TRUE;
}

// unit tests for hash
bool hash_test() {
    hash h1;
    hash h2;

    hash_create(&h1, "actual", 10);
    hash_create(&h2, "expected", 10);

    hash_add(&h1, 9);
    hash_add(&h1, 19);
    hash_remove(&h1, 9);

    hash_add(&h2, 19);

    if (hash_equal(&h1, &h2) != TRUE) {
        printf("test 1 failed!\n");
        hash_dump(&h1);
        hash_dump(&h2);
    } else {
        printf("test 1 ok.\n");
    }
    hash_free(&h1);
    hash_free(&h2);


    hash_create(&h1, "actual", 10);
    hash_create(&h2, "expected", 10);

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
        printf("test 2 failed!\n");
        hash_dump(&h1);
        hash_dump(&h2);
        return FALSE;
    } else {
        printf("test 2 ok.\n");
    }
    hash_free(&h1);
    hash_free(&h2);


    hash_create(&h1, "actual", 10);
    hash_create(&h2, "expected", 10);

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
        printf("test 3 failed!\n");
        hash_dump(&h1);
        hash_dump(&h2);
        return FALSE;
    } else {
        printf("test 3 ok.\n");
    }
    hash_free(&h1);
    hash_free(&h2);

    hash_create(&h1, "actual", 1000);
    int i = 0;
    while (i < h1.size) {
        hash_add(&h1, i++);
    }
    if (hash_add(&h1, i++) == TRUE) {
        printf("test 4 failed!\n");
    } else {
        printf("test 4 ok.\n");
    }
    hash_free(&h1);

    return TRUE;
}

int main(int argc, char *argv[]) {
    if (argc == 2 && strcmp("test", argv[1]) == 0) {
        if (hash_test() == TRUE) {
            printf("all tests passed successfully\n");
            return 0;
        }
        else {
            return 1;
        }
    }

    hash h1;

    char cmd[MAX_MSG_SIZE] = "";
    char cmd_short[MAX_MSG_SIZE] = "";
    HASH_KEY_TYPE key;

    hash_create(&h1, "test dyn", 5);

    hash_dump(&h1);

    while (readcmd("[a]dd N, [r]emove N, [q]uit: ", cmd)) {
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
                if (!hash_add(&h1, key)) {
                    printf("unable to add key [%d] to hash [%s]: no space left\n", key, h1.name);
                }
            }
            if (strcmp(cmd_short, "r") == 0) {
                if (!hash_remove(&h1, key)) {
                    printf("unable to remove key [%d] to hash [%s]: it's not there\n", key, h1.name);
                }
            }
        }

        hash_dump(&h1);
    }

    return 0;
}
