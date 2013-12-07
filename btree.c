#include "mystd.h"

// the size of nodes storage
#define BTREE_MAX_NODES 999

#define BTREE_T 3
#define BTREE_MIN_KEYS_IN_NODE (BTREE_T - 1)
#define BTREE_MAX_KEYS_IN_NODE (2 * BTREE_T - 1)
#define BTREE_MAX_CHILDREN_FOR_NODE (2 * BTREE_T)
#define BTREE_KEY_TYPE int

typedef struct __type_btree_node {
    // id of the node matching its number if global BTREE nodes storage
    int id;

    // number of keys currently stored in the node
    int n;

    // keys stored in the node
    BTREE_KEY_TYPE keys[BTREE_MAX_KEYS_IN_NODE];

    // whether given key is initialized or not
    bool keys_used[BTREE_MAX_KEYS_IN_NODE];

    // is this a leaf node
    bool leaf;
    
    // is this a root node
    bool root;

    // children nodes for root and internal nodes, should be empty for leafs
    struct __type_btree_node *children[BTREE_MAX_CHILDREN_FOR_NODE];
} btree_node;

typedef struct __type_btree {
    char name[STRUCT_NAME_LENGTH];
    btree_node *root;
} btree;

// global nodes storage so we don't have
// to allocate memory in runtime
btree_node BTREE_NODES[BTREE_MAX_NODES];

// which BTREE_NODES are currently used
bool BTREE_NODES_USED[BTREE_MAX_NODES];

// position (pointer, not in C terms) in the nodes storage
// to the first free node which could be used (allocated)
int BTREE_NODES_LAST_USED = 0;

// returns pointer to the free (unused) node which can be used
btree_node *allocate_node() {
    if (BTREE_NODES_LAST_USED + 1 == BTREE_MAX_NODES) {
        die("Unable to allocate new node");
    }

    int i = BTREE_NODES_LAST_USED;
    BTREE_NODES_LAST_USED++;

    BTREE_NODES_USED[i] = true;

    // node initialization
    BTREE_NODES[i].id = i;
    BTREE_NODES[i].n = 0;
    BTREE_NODES[i].root = false;
    BTREE_NODES[i].leaf = false;

    int j = 0;
    while(j < BTREE_MAX_KEYS_IN_NODE) {
        BTREE_NODES[i].keys_used[j] = 0;
        BTREE_NODES[i].keys_used[j] = false;
        BTREE_NODES[i].children[j] = NULL;
        j++;
    }
    // there's one more child is greater than number of keys 
    BTREE_NODES[i].children[j] = NULL;

    return &BTREE_NODES[i];
}

void free_node(btree_node *n) {
    // fill up the space which is going to free with the last used node
    if (BTREE_NODES_LAST_USED > 1) {
        // copy the last node to the free space
        BTREE_NODES[n->id] = BTREE_NODES[BTREE_NODES_LAST_USED - 1];
    
        // update its internal reference into the node storage
        BTREE_NODES[n->id].id = n->id;
    }

    // not updating any of the copied node elements, since
    // they all will be updated upon initialization in allocate_node()
    BTREE_NODES_LAST_USED--;
    return;
}

void dump_node_storage() {
    int max_length_of_btree_id;
    char *tmp_str;
    tmp_str = malloc(MAX_MSG_SIZE);
    sprintf(tmp_str, "%d", (int) BTREE_MAX_NODES);
    max_length_of_btree_id = strlen(tmp_str);

    int i = 0;
    printf("nodes storage: ");
    while (i < BTREE_NODES_LAST_USED) {
        printf("%0*d ", max_length_of_btree_id, i);
        i++;
    }
    printf("\n");
}

void btree_init(btree *t, char *name) {
    if (strlen(name) < 1) {
        die("btree_create: need btree name");
    }
    else if (strlen(name) > STRUCT_NAME_LENGTH) {
        char *msg;
        msg = malloc(MAX_MSG_SIZE);
        sprintf(msg, "btree_create: btree name must be no longer than %d", STRUCT_NAME_LENGTH);
        die(msg);
        free(msg);
    }

    strcpy(t->name, name);

    t->root = allocate_node();
    t->root->leaf = true;
    t->root->root = true;

    return;
}

void btree_split_child(btree_node *t, int child_number) {
}

void btree_insert_nonfull(btree_node *t, BTREE_KEY_TYPE k) {
}

void btree_insert(btree *t, BTREE_KEY_TYPE k) {
    btree_node *r;
    r = t->root;

    if (r->n == BTREE_MAX_KEYS_IN_NODE) {
        btree_node *s;
        s = allocate_node();
        t->root = s;
        s->leaf = false;
        s->children[0] = r;

    }
    else {
        btree_insert_nonfull(t->root, k);
    }
}

int main(int argc, char *argv[]) {
    btree t;

    btree_init(&t, "tree 1");

    char cmd[MAX_MSG_SIZE] = "";
    char cmd_short[MAX_MSG_SIZE] = "";
    int key;

    while (readcmd("[a]dd N, [r]emove N, [p]rint, [d]ump node storage, [q]uit: ", cmd)) {
//        printf("got [%s]\n", cmd);
        if (strcmp(cmd, "q") == 0) {
            printf("bye\n");
            break;
        }
        if (strcmp(cmd, "d") == 0) {
            dump_node_storage();
        }

        if (strstr(cmd, "a") == &cmd[0] || strstr(cmd, "r") == &cmd[0]) {
            if (sscanf(cmd, "%s %d", cmd_short, &key) != 2) {
                printf("invalid command: %s\n", cmd);
                continue;
            }
//        printf("got: [%s] [%d]\n", cmd_short, key);
            printf("\n");

            if (strcmp(cmd_short, "a") == 0) {
//                hash_add(&h1, key);
            }
            if (strcmp(cmd_short, "r") == 0) {
//                hash_remove(&h1, key);
            }
        }

//        hash_dump(&h1);
    }
    
    return 0;
}
