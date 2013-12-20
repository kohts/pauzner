//
// b-tree implementation as described in Introduction to Algorithms, 3rd edition
//
// b-tree nodes storage uses static memory, size of which along with the
// branching factor of b-tree is configured at compile time for now
// (should switch to dynamic memory allocation to get ready for production)
//

#include "mystd.h"
#include <math.h>

// the size of nodes storage
#define BTREE_MAX_NODES 999

#define BTREE_T 2
#define BTREE_MIN_KEYS_IN_NODE (BTREE_T - 1)
#define BTREE_MAX_KEYS_IN_NODE (2 * BTREE_T - 1)
#define BTREE_MAX_CHILDREN_FOR_NODE (2 * BTREE_T)
#define BTREE_KEY_TYPE int

typedef struct __type_btree_node {
    // id of the node matching its number in global BTREE nodes storage
    int id;

    // number of keys currently stored in the node; 1-based
    int n;

    // keys stored in the node
    BTREE_KEY_TYPE keys[BTREE_MAX_KEYS_IN_NODE];

    // whether given key is initialized or not
    boolean keys_used[BTREE_MAX_KEYS_IN_NODE];

    // is this a leaf node
    boolean leaf;
    
    // is this a root node
    boolean root;

    // children nodes for root and internal nodes, should be empty for leafs
    struct __type_btree_node *children[BTREE_MAX_CHILDREN_FOR_NODE];
} btree_node;

typedef struct __type_btree {
    char name[STRUCT_NAME_LENGTH];
    btree_node *root;
    int height;
} btree;

typedef struct __type_btree_search_result {
    btree_node *node;
    int matching_key_index;
} btree_search_result;

typedef struct __type_btree_node_key_search_result {
    boolean found;
    int position;
    int left_boundary;
    int right_boundary;
} btree_node_key_search_result;

// global nodes storage so we don't have
// to allocate memory in runtime
btree_node BTREE_NODES[BTREE_MAX_NODES];

// which BTREE_NODES are currently used
boolean BTREE_NODES_USED[BTREE_MAX_NODES];

// position (pointer, not in C terms) in the nodes storage
// to the first free node which could be used (allocated)
int BTREE_NODES_LAST_USED = 0;

// forward declarations
void btree_init(btree *t, char *name);
void btree_insert(btree *tree, BTREE_KEY_TYPE key);
void btree_insert_nonfull(btree_node *node, BTREE_KEY_TYPE key);
void btree_split_child(btree_node *parent_node, int child_to_split_number);
btree_search_result *btree_search(btree *tree, BTREE_KEY_TYPE key, btree_search_result *result);
btree_search_result *btree_search_node(btree_node *tree, BTREE_KEY_TYPE key, btree_search_result *result);
boolean btree_remove(btree *tree, BTREE_KEY_TYPE key);

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
        BTREE_NODES[i].keys[j] = 0;
        BTREE_NODES[i].keys_used[j] = false;
        BTREE_NODES[i].children[j] = NULL;
        j++;
    }
    // there's one more child is greater than number of keys 
    BTREE_NODES[i].children[j] = NULL;

    return &BTREE_NODES[i];
}

void free_node(btree_node *n) {
    // fill up the space which is being freed in the global nodes array
    // by the last allocated node (copy it into the freed space)
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

void btree_node_dump(btree_node *x, boolean print_node_id) {
    printf(" ");
    
    if (print_node_id == true) {
        printf("node%d:", x->id);
    }

    int i;
    for(i=0; i < x->n; i++) {
        printf("%d", x->keys[i]);
//        printf("%d (%s)", x->keys[i], (x->keys_used[i] == true ? "y" : "n"));
        if (i < x->n - 1) {
            printf("_");
        }
    }
}

int btree_print_nodes_by_level (btree_node *x, int required_level, int current_level, boolean print_node_id) {
    int i;

    if (required_level == current_level) {
        btree_node_dump(x, print_node_id);
        return 1;
    }
    
    int nodes_printed = 0;
    for (i = 0; i <= x->n; i++) {
        nodes_printed = nodes_printed + btree_print_nodes_by_level(x->children[i], required_level, current_level + 1, print_node_id);
    }
    return nodes_printed;
}

void btree_dump(btree *tree, boolean print_node_id) {
    printf("\nDumping btree [%s]:\n", tree->name);

    btree_node *x;
    x = tree->root;

    int j;
    for (j = 1; j <= tree->height; j++) {
        printf("h %d: ", j);
        btree_print_nodes_by_level(x, j, 1, print_node_id);
        printf("\n");
    }

    printf("\n\n");
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

    t->height = 1;

    return;
}

// insert the key into the node
//
// functions assumes that:
//
//    * the node contains no more than 2t-1 keys (which is guaranteed
//      by the btree_split_child() run before btree_insert_nonfull()
//
//    * the key being insert is not present in the tree which is easily
//      solved for 1 CPU by doing (fast) btree_search before running
//      btree_insert() but might become a problem if the tree
//      is written simultaneously by several CPU(s)
//
void btree_insert_nonfull(btree_node *node, BTREE_KEY_TYPE key) {
    int i;

    i = node->n;

    if (node->leaf == true) {
        // shift all the keys right by one position
        // to prepare space for the key which is being inserted
        while (i >= 1 && key < node->keys[(i) - 1]) {
            node->keys[(i + 1) - 1] = node->keys[(i) - 1];
            node->keys_used[(i + 1) - 1] = true;
            i--;
        }

        // insert the key into prepared position
        node->keys[(i + 1) - 1] = key;
        node->keys_used[(i + 1) - 1] = true;

        // increment number of keys in the node
        node->n = node->n + 1;
    
        // write node to the disk
    }
    else {
        // find the child which holds the key; should change
        // from linear search to binary search to gain speed
        while (i >= 1 && key < node->keys[(i) - 1]) {
            i = i - 1;
        }
        i = i + 1;
        
        // read child from disk

        if (node->children[(i) - 1]->n == BTREE_MAX_KEYS_IN_NODE) {
            btree_split_child(node, (i) - 1);
           
            if (key > node->keys[(i) - 1]) {
                i = i + 1;
            }
        }

        btree_insert_nonfull(node->children[(i) - 1], key); 
    }
}

// split node which contains 2t-1 keys into two nodes
// with t-1 keys each and pop one key up to the parent_node
// (t-1) + (t-1) + 1 == 2t - 2 + 1 == 2t - 1
//
// note: child_number is 0-based (as opposed to the book notation),
// so it is adjusted by 1 in the code (I'm using parenthesis
// to separate algorithm logic from 0-based adjustment)
// 
void btree_split_child(btree_node *parent_node, int child_to_split_number) {
    btree_node *z, *y;

    y = parent_node->children[child_to_split_number];

    z = allocate_node();
    z->leaf = y->leaf;

    // copy the keys starting with t+1 (human) from y to z
    // and also the children pointers (if they exist)
    //
    // C     0    1    2    3    4
    // human 1    2    3    4    5
    //
    //       t-1  t    2t-2 2t-1 2t
    //       2    3    4    5    6
    //
    int j;
    int i;
    for (j = (1) - 1; j <= (BTREE_T - 1) - 1; j++) {
        
        // position of the element to be copied from y to z
        // (j=0 => i=BTREE_T which is 0-based t+1 (t+1-1) 
        i = j + BTREE_T;

        z->keys[j] = y->keys[i];
        z->keys_used[j] = true;
        y->keys_used[i] = false;

        if (y->leaf == false) {
            z->children[j] = y->children[i];
        }
    }
    // there's one more child
    if (y->leaf == false) {
        // j == BTREE_T - 1 (which is 0-based equiv for 1-based t)
        // i == 2t - 1, which is 0-based equiv for (2t-1)+1 -- (2t-1+1-1)
        i = j + BTREE_T;
        z->children[j] = y->children[i];
    }

    z->n = BTREE_T - 1; // 1-based
    y->n = BTREE_T - 1; // 1-based

    // prepare space for the new key (which is about to pop up)
    // in the parent by shifting its keys and children one to the right
    for (j = (parent_node->n) - 1; j >= child_to_split_number; j--) {
        parent_node->keys[j + 1] = parent_node->keys[j];
        parent_node->keys_used[j + 1] = true;
        parent_node->children[(j + 1) + 1] = parent_node->children[(j) + 1];
    }
    // insert new key into parent_node->keys in place of moved key
    parent_node->keys[child_to_split_number] = y->keys[(BTREE_T) - 1];
    parent_node->keys_used[child_to_split_number] = true;
    
    // insert pointer to z into the parent_node->children
    parent_node->children[child_to_split_number + 1] = z;
    
    // new key (and new child) inserted, increment number of keys in the node
    parent_node->n = parent_node->n + 1;

    // write y to the disk
    // write z to the disk
    // write parent_node to the disk

    return;
}

// this is the main function to insert a key into btree
//
void btree_insert(btree *tree, BTREE_KEY_TYPE key) {
    btree_search_result *presult;
    btree_search_result result;

    presult = btree_search(tree, key, &result);
    if (presult != NULL) {
        printf("key [%d] already exists in the tree (node %d), unable to insert\n", key, presult->node->id);
        return;
    }

    btree_node *r;
    r = tree->root;

    if (r->n == BTREE_MAX_KEYS_IN_NODE) {
        // the tree is going to grow, the old root is no longer theroot
        r->root = false;

        // prepare new root
        btree_node *s;
        s = allocate_node();
        s->leaf = false;
        s->root = true;
        
        // make old root the first (and the only child for now) of the new root
        s->children[0] = r;

        // update the root node pointer in the tree object
        tree->root = s;

        tree->height = tree->height + 1;
        
        // this will split the old root into two internal nodes
        btree_split_child(s, 0);

        // now we can continue inserting the key into btree
        btree_insert_nonfull(tree->root, key);
    }
    else {
        // the root doesn't have to grow (less than 2t-1 keys in it),
        // so simply insert the key
        btree_insert_nonfull(tree->root, key);
    }
}

// find either a key in the node keys array and/or its neighbours.
//
// currently implemented as a linear search returning result as static variable,
// TODO: should be reimplemented as a binary search with dynamic memory result
//
// returns structure with the following fields:
//    found -- true/false
//    position -- zero-based key position
//    left_boundary -- zero-based left neighbour of the key
//    right_boundary -- zero-based right neighbour of the key
//
//  when key is found
//    * position holds index in the node->keys array
//    * left_boundary holds either preceding index or -1 (if the key
//      is at the leftmost position),
//    * rifht_boundary holds either succeeding index or -1 (if the key
//      is at the rightmost position)
//  when key is not found
//    * position holds -1
//    * left_boundary holds either -1 (when searched key is to the left
//      of the first key in the node) or zero-based position of the left neighbour
//      of the searched key
//    * right_boundary holds either -1 (when searched key is to the right
//      of the last key in the node) or zero-based position of the right neighbour
//
btree_node_key_search_result btree_node_find_key(btree_node *node, BTREE_KEY_TYPE key) {
    btree_node_key_search_result r;
    r.found = false;
    r.position = -1;
    r.left_boundary = -1;
    r.right_boundary = -1;

    boolean finish_search = false;
    int i;
    
    for(i=0; finish_search != true && i < node->n; i++) {
        if (node->keys[i] == key) {
            r.found = true;
            r.position = i;
            if (i > 0) {
                r.left_boundary = i - 1;
            }
            if (i < node->n - 1) {
                r.right_boundary = i + 1;
            }
        
            finish_search = true;
        }
        else {
            if (i == 0) {
                if (key < node->keys[i]) {
                    r.right_boundary = i;
                    finish_search = true;
                }
            }
            
            if (i == node->n - 1) {
                if (key > node->keys[i]) {
                    r.left_boundary = i;
                    finish_search = true;
                }
            }

            if (i < node->n - 1) {
                if (node->keys[i] < key && key < node->keys[i+1]) {
                    r.left_boundary = i;
                    r.right_boundary = i+1;
                    finish_search = true;
                }
            }
        }
    }

    return r;
}

// search for the next key == the smallest key being greater than given key
// (which might not exist in the (sub)tree)
//
// returns the structure which contains:
//    * node -- pointer to the node where next key was found (or NULL)
//    * matching_key_index -- zero-based position of the next key in the node
// 
btree_search_result btree_find_next(btree_node *node, BTREE_KEY_TYPE key) {
    btree_search_result tree_r;
    btree_node_key_search_result key_r;

    key_r = btree_node_find_key(node, key);

    if (node->leaf == true) {
        // we don't care whether we've found the key, because we only need
        // its right_boundary (if it exists in the leaf node)
        if (key_r.right_boundary != -1) {
            tree_r.node = node;
            tree_r.matching_key_index = key_r.right_boundary;
        }
        else {
            tree_r.node = NULL;
        }
    }
    else {
        if (key_r.found == true) {
            // found a key on this level, descend into "right" child
            tree_r = btree_find_next(node->children[key_r.position + 1], key);
        }
        else {
            // didn't found a key on this level
            //
            if (key_r.left_boundary == -1) {
                // next key might only be in the first child
                tree_r = btree_find_next(node->children[0], key);
            }
            else if (key_r.right_boundary == -1) {
                // next key might only be in the last child
                tree_r = btree_find_next(node->children[node->n], key);
            }
            else {
                // next key is in the child which is pointed to by either
                // key_r.left_boundary (or which is the same key_r.right_boundary - 1)
                tree_r = btree_find_next(node->children[key_r.left_boundary], key);
            }

            // if next key was not found on the level below, then we can assume
            // it's the right_boundary (if it exist)
            if (tree_r.node == NULL && key_r.right_boundary != -1) {
                tree_r.node = node;
                tree_r.matching_key_index = key_r.right_boundary;
            }
        }
    }

    return tree_r;
}

// search for the previous key == the largest key being less than given key
// (which might not exist in the (sub)tree)
//
// returns the structure which contains:
//    * node -- pointer to the node where previous key was found (or NULL)
//    * matching_key_index -- zero-based position of the previous key in the node
//
btree_search_result btree_find_previous(btree_node *node, BTREE_KEY_TYPE key) {
    btree_search_result tree_r;
    btree_node_key_search_result key_r;

    key_r = btree_node_find_key(node, key);

    if (node->leaf == true) {
//        printf ("leaf %d -- %d\n", key_r.left_boundary, key_r.right_boundary);

        // we don't care whether we've found the key, because we only need
        // its left_boundary (if it exists in the leaf node)
        if (key_r.left_boundary != -1) {
            tree_r.node = node;
            tree_r.matching_key_index = key_r.left_boundary;
        }
        else {
            tree_r.node = NULL;
        }
    }
    else {
        if (key_r.found == true) {
            // found a key on this level, descend into "left" child
            tree_r = btree_find_previous(node->children[key_r.position], key);
        }
        else {
//            printf ("internal node %d -- %d\n", key_r.left_boundary, key_r.right_boundary);

            // didn't found a key on this level
            //
            if (key_r.left_boundary == -1) {
                // previous key might only be in the first child
                tree_r = btree_find_previous(node->children[0], key);
            }
            else if (key_r.right_boundary == -1) {
                // previous key might only be in the last child
                tree_r = btree_find_previous(node->children[node->n], key);
            }
            else {
                // previous key is in the child which is pointed to by either
                // key_r.left_boundary + 1 (or which is the same key_r.right_boundary)
                tree_r = btree_find_previous(node->children[key_r.left_boundary + 1], key);
            }

            // if previous key was not found on the level below, then we can assume
            // it's the left_boundary (if it exist)
            if (tree_r.node == NULL && key_r.left_boundary != -1) {
                tree_r.node = node;
                tree_r.matching_key_index = key_r.left_boundary;
            }
        }
    }
    
    return tree_r;
}

// deletes the key from subtree rooted at node.
//
// the procedure assumes that it is never called on a node
// with number of keys in it being less than BTREE_T (with one
// exception being the root node)
//
// returns TRUE when the key was deleted or FALSE otherwise.
//
boolean btree_remove_node(btree *tree, btree_node *node, BTREE_KEY_TYPE key) {
    // no keys in the node
    if (node->n == 0) {
        if (node->leaf == false) {
            // if the root node x ever becomes an internal node having no keys
            // (this situation can occur in cases 2c and 3b on pages 501-502),
            // then we delete x, and x's only child x:c1 becomes the new root of the tree,
            // decreasing the height of the tree by one and preserving the property
            // that the root of the tree contains at least one key (unless the tree is empty).
            if (node->children[0] == NULL) {
                die("btree_remove_node: internal node [%d] has no keys and no children; btree_implementation error\n");
//                die("btree_remove_node: internal node [%d] has no keys and no children; btree_implementation error\n", node->id);
            }

            tree->root = node->children[0];
            free_node(node);
        }
        
        return false;
    }

    int i;
    btree_node_key_search_result key_r;

    if (node->leaf == true) {
        // 1. If the key k is in node x and x is a leaf, delete the key k from x.
        key_r = btree_node_find_key(node, key);

        if (key_r.found == true) {
            // shift all the keys to the left by one starting
            // with the found position
            for (i=key_r.position; i < node->n - 1; i++) {
                node->keys[i] = node->keys[i+1];
            }
            node->keys_used[node->n - 1] = false;
            node->n = node->n - 1;
            return true;
        }
        else {
            return false;
        }
    }
    else {
        key_r = btree_node_find_key(node, key);

        if (key_r.found == true) {
            // 2. If the key k is in node x and x is an internal node
        
            if (node->children[key_r.position]->n >= BTREE_T) {
                // 2a. If the child y that precedes k in node x has at least t keys
                // then find the predecessor k0 of k in the subtree rooted at y.
                
            }
        }
        else {
            // 3. If the key k is not present in internal node x
        }

        return false;
    }

    return false;
}

// this is a main function to delete a key from btree
//
boolean btree_remove(btree *tree, BTREE_KEY_TYPE key) {
    return btree_remove_node(tree, tree->root, key);
}

// find a key in the btree starting with the given node using binary search
//
btree_search_result *btree_search_node(btree_node *node, BTREE_KEY_TYPE key, btree_search_result *result) {
    int left_boundary = 0;
    int right_boundary = BTREE_MAX_KEYS_IN_NODE - 1;
    int center;

    boolean found_key = false;
    boolean found_key_position;
    boolean done_searching = false;

    while(done_searching != true) {
        if (node->keys_used[left_boundary] == false) {
            if (node->leaf == true) {
                done_searching = true;
                continue;
            }
            else {
                if (left_boundary > 0) {
                    // left boundary moved to center + 1 on the previous cycle,
                    // but it's not set (while previous center was), since it's
                    // not the leaf - descend into previous center "right" child
                    return btree_search_node(node->children[left_boundary], key, result);
                }
                else {
                    done_searching = true;
                    continue;
                }
            }
        }

        //   left right (right-left)/5 center
        //   0    1     0.5            1
        // b 0    1     0.5            1 
        // c 1    1     0              1
        //
        //   0    2     1              1
        // b 0    1
        // c 1    2     1              2
        //
        //   0    3     1.5            2
        // b 0    2
        // c 2    3     0.5            3
        // 
        //   1    2     0.5            2
        // b 1    1
        // c 2    2
        //
        center = left_boundary + round((right_boundary - left_boundary) / 2);

//        printf("%d <-- %d --> %d\n", left_boundary, center, right_boundary);

        if (node->keys_used[center] == false) {
            right_boundary = center - 1;
            continue;
        }

        // (a), (b) and (c) cover all the possible cases
        
        // (a)
        if (node->keys[center] == key) {
            found_key = true;
            found_key_position = center;
            done_searching = true;
            continue;
        }
        
        // (d): left_boundary == right_boundary == center
        if (left_boundary == right_boundary) {
            if (node->leaf == true) {
                done_searching = true;
                continue;
            }
            else {
                if (key < node->keys[center]) {
                    return btree_search_node(node->children[center], key, result);
                }
                if (node->keys[center] < key) {
                    return btree_search_node(node->children[center + 1], key, result);
                }

                die("btree_search_node: something is very wrong; left %d, right %d, center %d, node id %d");
//                die("btree_search_node: something is very wrong; left %d, right %d, center %d, node id %d", left_boundary, right_boundary, center, node->id);
            }
        }
        
        // (b)
        if (key < node->keys[center]) {
            right_boundary = center - 1;
            continue;
        }

        // (c)
        if (node->keys[center] < key) {
            left_boundary = center + 1;
            continue;
        }

        die("btree_search_node: binary_search has an error; left %d, right %d, center %d, node id %d");
    }

    if (found_key == true) {
        result->node = node;
        result->matching_key_index = found_key_position;
        return result;
    }
    else {
        return NULL;
    }
}

// takes btree and returns either NULL when key is not present in the btree
// or struct which holds the node pointer and index of the key
// in the result->node->keys array
//
btree_search_result *btree_search(btree *t, BTREE_KEY_TYPE key, btree_search_result *result) {
    return btree_search_node(t->root, key, result);
}

boolean btree_equal(btree *t1, btree *t2) {
    return true;
}

boolean btree_test() {
    btree t1;
    btree t2;

    btree_init(&t1, "actual");
    btree_init(&t2, "expected");

    btree_insert(&t1, 1);
    btree_insert(&t1, 10);
    btree_insert(&t1, 1000);
    btree_insert(&t1, 50);
    btree_insert(&t1, 3);
    btree_insert(&t1, 15);

    return true;
}

int main(int argc, char *argv[]) {
    if (argc == 2 && strcmp("test", argv[1]) == 0) {
        if (btree_test() == TRUE) {
            printf("tests passed\n");
            return 0;
        }
        else {
            return 1;
        }
    }

    btree t;
    btree_search_result *presult;
    btree_search_result result;

    btree_init(&t, "tree 1");
    btree_dump(&t, false);

    char cmd[MAX_MSG_SIZE] = "";
    char cmd_short[MAX_MSG_SIZE] = "";
    BTREE_KEY_TYPE key;

    while (readcmd("[a]dd N, [r]emove N, [p]rint, [d]ump node storage, [sp]revious N, [q]uit: ", cmd)) {
//        printf("got [%s]\n", cmd);
        if (strcmp(cmd, "q") == 0) {
            printf("bye\n");
            break;
        }
        if (strcmp(cmd, "d") == 0) {
            dump_node_storage();
            btree_dump(&t, true);
            continue;
        }

        if (
            strstr(cmd, "a") == &cmd[0] ||
            strstr(cmd, "r") == &cmd[0] ||
            strstr(cmd, "s") == &cmd[0] ||
            strstr(cmd, "sp") == &cmd[0] ||
            strstr(cmd, "sn") == &cmd[0]
            ) {
            if (sscanf(cmd, "%s %d", cmd_short, &key) != 2) {
                printf("invalid command: %s\n", cmd);
                continue;
            }
//        printf("got: [%s] [%d]\n", cmd_short, key);
            printf("\n");

            if (strcmp(cmd_short, "a") == 0) {
                btree_insert(&t, key);
            }
            if (strcmp(cmd_short, "r") == 0) {
                btree_remove(&t, key);
            }
            if (strcmp(cmd_short, "s") == 0) {
                presult = btree_search(&t, key, &result);
                if (presult != NULL) {
                    printf("found key [%d], node [%d]!\n", key, presult->node->id);
                }
                else {
                    printf("key [%d] not found in the tree!\n", key);
                }
                btree_dump(&t, true);
                continue;
            }
            if (strcmp(cmd_short, "sp") == 0) {
                result = btree_find_previous(t.root, key);
                if (result.node != NULL) {
                    printf("found previous key [%d] for the key [%d] in node [%d]!\n",
                        result.node->keys[result.matching_key_index], key, result.node->id);
                }
                else {
                    printf("no previous key found for key [%d] in the tree!\n", key);
                }

                btree_dump(&t, true);
                continue;
            }
            if (strcmp(cmd_short, "sn") == 0) {
                result = btree_find_next(t.root, key);
                if (result.node != NULL) {
                    printf("found next key [%d] for the key [%d] in node [%d]!\n",
                        result.node->keys[result.matching_key_index], key, result.node->id);
                }
                else {
                    printf("no next key found for key [%d] in the tree!\n", key);
                }

                btree_dump(&t, true);
                continue;
            }
        }

        btree_dump(&t, false);
    }
    
    return 0;
}
