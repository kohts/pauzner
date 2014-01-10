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

// btree node
typedef struct __type_btree_node {
    // id of the node matching its number in global BTREE nodes storage
    int id;

    // number of keys currently stored in the node; 1-based
    int n;

    // keys stored in the node
    BTREE_KEY_TYPE keys[BTREE_MAX_KEYS_IN_NODE];

    // is this a leaf node
    boolean leaf;
    
    // is this a root node
    boolean root;

    // children nodes for root and internal nodes, should be empty for leafs
    struct __type_btree_node *children[BTREE_MAX_CHILDREN_FOR_NODE];
} btree_node;

// btree
typedef struct __type_btree {
    char name[STRUCT_NAME_LENGTH];
    btree_node *root;
    int height;
} btree;

// the result of searching a btree for specific key
//    * search result flag (true/false)
//    * pointer to the node containing the key
//    * index of the key in the node->keys array
//    * found key (convenience feature)
//
typedef struct __type_btree_search_result {
    boolean found;
    btree_node *node;
    int matching_key_index;
    BTREE_KEY_TYPE key;
} btree_search_result;

// the result of searching a node for a key;
// for details read comments for btree_node_find_key()
// which returns this structure
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
void btree_create(btree *t, char *name);
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

// dumps whole nodes storage (not just assigned to any specific btree)
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

// dumps btree node, optionally dumps node id (which specifies
// node place in the global nodes storage)
void btree_node_dump(btree_node *x, boolean print_node_id) {
    printf("  ");
    
    if (print_node_id == true) {
        printf("node%d:", x->id);
    }

    int i;
    for(i=0; i < x->n; i++) {
        printf("%d", x->keys[i]);
        if (i < x->n - 1) {
            printf("_");
        }
    }
}

// dumps nodes which are at the required_level depth in the tree,
// returns total number of nodes at this level (height, depth)
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

void btree_create(btree *t, char *name) {
    if (strlen(name) < 1) {
        die("btree_create: need btree name");
    }
    else if (strlen(name) > STRUCT_NAME_LENGTH) {
        die("btree_create: btree name must be no longer than %d", STRUCT_NAME_LENGTH);
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
            i--;
        }

        // insert the key into prepared position
        node->keys[(i + 1) - 1] = key;

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
        parent_node->children[(j + 1) + 1] = parent_node->children[(j) + 1];
    }
    // insert new key into parent_node->keys in place of moved key
    parent_node->keys[child_to_split_number] = y->keys[(BTREE_T) - 1];
    
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

// find a key in the node keys array and/or this key neighbours.
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
//  when key is found in the node:
//    * position holds index in the node->keys array
//    * left_boundary holds either preceding index or -1 (if the key
//      is at the leftmost position),
//    * rifht_boundary holds either succeeding index or -1 (if the key
//      is at the rightmost position)
//
//  when key is not found in the node:
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
// returns btree_search_result structure (described in header)
// 
btree_search_result btree_find_next(btree_node *node, BTREE_KEY_TYPE key) {
    btree_search_result tree_r;
    btree_node_key_search_result key_r;

    tree_r.found = false;
    tree_r.node = NULL;
    
    key_r = btree_node_find_key(node, key);

    if (node->leaf == true) {
        // we don't care whether we've found the key, because we only need
        // its right_boundary (if it exists in the leaf node)
        if (key_r.right_boundary != -1) {
            tree_r.found = true;
            tree_r.node = node;
            tree_r.matching_key_index = key_r.right_boundary;
            tree_r.key = tree_r.node->keys[tree_r.matching_key_index];
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
            if (tree_r.found == false && key_r.right_boundary != -1) {
                tree_r.found = true;
                tree_r.node = node;
                tree_r.matching_key_index = key_r.right_boundary;
                tree_r.key = tree_r.node->keys[tree_r.matching_key_index];
            }
        }
    }

    return tree_r;
}

// search for the previous key == the largest key being less than given key
// (which might not exist in the (sub)tree)
//
// returns btree_search_result structure (described in header)
//
btree_search_result btree_find_previous(btree_node *node, BTREE_KEY_TYPE key) {
    btree_search_result tree_r;
    btree_node_key_search_result key_r;

    tree_r.found = false;
    tree_r.node = NULL;

    key_r = btree_node_find_key(node, key);

    if (node->leaf == true) {
//        printf ("leaf %d -- %d\n", key_r.left_boundary, key_r.right_boundary);

        // we don't care whether we've found the key, because we only need
        // its left_boundary (if it exists in the leaf node)
        if (key_r.left_boundary != -1) {
            tree_r.found = true;
            tree_r.node = node;
            tree_r.matching_key_index = key_r.left_boundary;
            tree_r.key = tree_r.node->keys[tree_r.matching_key_index];
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
            if (tree_r.found == false && key_r.left_boundary != -1) {
                tree_r.found = true;
                tree_r.node = node;
                tree_r.matching_key_index = key_r.left_boundary;
                tree_r.key = tree_r.node->keys[tree_r.matching_key_index];
            }
        }
    }
    
    return tree_r;
}

// appends key and source_node keys (and children if any) to the target_node
// 
void btree_append_adjacent_node (btree_node *target_node, btree_node *source_node, BTREE_KEY_TYPE key) {
    if (target_node->leaf != source_node->leaf) {
        die("Programmer error: btree_append_adjacent_node expects nodes of the same level");
    }

    target_node->keys[target_node->n] = key;
    target_node->n = target_node->n + 1;

    if (target_node->leaf == false) {
        target_node->children[target_node->n] = source_node->children[0];
    }

    int i;
    for(i=0; i < source_node->n; i++) {
        target_node->keys[target_node->n] = source_node->keys[i];
        target_node->n = target_node->n + 1;
        
        if (target_node->leaf == false) {
            target_node->children[target_node->n] = source_node->children[i+1];
        }
    }
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
            node->n = node->n - 1;
            return true;
        }
        else {
            return false;
        }
    }
    else {
        btree_node *left_child, *right_child;
        btree_search_result search_r;
        boolean removed_k0;

        key_r = btree_node_find_key(node, key);

        if (key_r.found == true) {
            // 2. If the key k is in node x and x is an internal node
        
            if (node->children[key_r.position]->n >= BTREE_T) {
                // 2a. If the child y that precedes k  in node x has at least t keys
                // then find the predecessor k0 of k in the subtree rooted at y.
                search_r = btree_find_previous(node->children[key_r.position], key);
                if (search_r.found == true) {
                    // Recursively delete k0
                    removed_k0 = btree_remove_node(tree, node->children[key_r.position], search_r.key);
                    if (removed_k0 == false) {
                        die("btree_remove_node: unable to remove just found key; error in btree implementation");
                    }

                    // and replace k by k0 in x.
                    node->keys[key_r.position] = search_r.key;
                }
                else {
                    die("btree_remove_node: unable to find previous key for the given key in the left child with some keys; error in btree implementation");
                }
            }
            else if (node->children[key_r.position + 1]->n >= BTREE_T) {
                // 2b. If y has fewer than t keys, then, symmetrically, examine the child z that
                // follows k in node x. If z has at least t keys then find the successor k0 of k in
                // the subtree rooted at z.
                search_r = btree_find_next(node->children[key_r.position + 1], key);
                if (search_r.found == true) {
                    // Recursively delete k0
                    removed_k0 = btree_remove_node(tree, node->children[key_r.position + 1], search_r.key);
                    if (removed_k0 == false) {
                        die("btree_remove_node: unable to remove just found key; error in btree implementation");
                    }

                    // and replace k by k0 in x.
                    node->keys[key_r.position] = search_r.key;
                }
                else {
                    die("btree_remove_node: unable to find next key for the given key in the right child with some keys; error in btree implementation");
                }
            }
            else {
                // Otherwise, if both y (left) and z (right) children have only t-1 keys,
                // merge k and all of z (right child) into y (left child), so that x (current node)
                // loses key and pointer to z (right child) and y (left child) contains 2t-1 keys,
                // then free z (right child) and recursively delete key from the left child.
                left_child = node->children[key_r.position];
                right_child = node->children[key_r.position+1];

                // merge k and both children into one node
                btree_append_adjacent_node(left_child, right_child, key);

                free_node(right_child);

                // remove key and right_child from the node
                for(i=key_r.position; i < node->n - 1; i++) {
                    node->keys[i] = node->keys[i+1];
                    node->children[i+1] = node->children[i+2];
                }
                node->n = node->n - 1;

                if (node->root == true && node->n == 0) {
                    tree->height = tree->height - 1;
                    tree->root = left_child;
                    left_child->root = true;
                    free_node(node);
                }

                return btree_remove_node(tree, left_child, key);
            }
        }
        else {
            // 3. If the key k is not present in internal node x determine the root
            // of the appropriate subtree that must contain k (x0)
            btree_node *target_node;

            if (key_r.left_boundary == -1) {
                target_node = node->children[key_r.right_boundary];
                left_child = NULL;
                right_child = node->children[key_r.right_boundary + 1];
            }
            else if (key_r.right_boundary == -1) {
                target_node = node->children[key_r.left_boundary + 1];
                left_child = node->children[key_r.left_boundary];
                right_child = NULL;
            }
            else {
                target_node = node->children[key_r.right_boundary];
                left_child = node->children[key_r.left_boundary];
                right_child = node->children[key_r.right_boundary + 1];
            }

            // if x0 has only t-1 keys
            if (target_node->n == BTREE_T - 1) {                
                // (3a) but has an immediate sibling with at least t keys, give it
                // an extra key by moving a key from x down into the x0,
                // moving the first key from the sibling up to x to the empty place
                // and moving child of the first key to the sibling's added key
                
                if (left_child != NULL && left_child->n >= BTREE_T) {
                    // shift keys (and children) one to the right in the target node,
                    // preparing the place for the key from current node
                    for(i = target_node->n - 1; i>=0; i--) {
                        target_node->keys[i+1] = target_node->keys[i];
                        target_node->children[i+2] = target_node->children[i+1];
                    }
                    target_node->children[1] = target_node->children[0];

                    // move key from node down to the target_node
                    target_node->keys[0] = node->keys[key_r.left_boundary];

                    // move key up from donor node in place of key which just moved down
                    node->keys[key_r.left_boundary] = left_child->keys[left_child->n - 1];

                    // move right child of the moved up key to the target_node
                    target_node->children[0] = left_child->children[left_child->n];

                    // update node counters for target node and donor node
                    target_node->n = target_node->n + 1;
                    left_child->n = left_child->n - 1;
                }
                else if (right_child != NULL && right_child->n >= BTREE_T) {
                    // move the key down from this node to target_node
                    target_node->keys[target_node->n] = node->keys[key_r.right_boundary];

                    // move first key from donor node up to the empty place in this node
                    node->keys[key_r.right_boundary] = right_child->keys[0];

                    // move left child of the moved key from donor node to the target_node
                    target_node->children[target_node->n + 1] = right_child->children[0];

                    // contract keys and children of the donor node by one (shifting left)
                    for(i = 0; i < right_child->n - 1; i++) {
                        right_child->keys[i] = right_child->keys[i+1];
                        right_child->children[i] = right_child->children[i+1];
                    }
                    right_child->children[right_child->n - 1] = right_child->children[right_child->n];

                    // update node counters for target node and donor node
                    target_node->n = target_node->n + 1;
                    right_child->n = right_child->n - 1;
                }
                else {
                    // (3b) If target_node and both of its immediate siblings have t-1 keys
                    // (or one of its siblings does not exist), merge target_node
                    // with one existing sibling which involves moving a key from current node
                    // down into the merged node, where it becomes the median key for that node.

                    if (left_child != NULL) {
                        btree_append_adjacent_node(left_child, target_node, node->keys[key_r.left_boundary]);
                       
                        free_node(target_node);
                        target_node = left_child;

                        // shrink this node using the empty place where node->keys[key_r.left_boundary] was
                        for (i=key_r.left_boundary; i < node->n - 1; i++) {
                            node->keys[i] = node->keys[i+1];
                            node->children[i+1] = node->children[i+2];
                        }
                        node->n = node->n - 1;
                    }
                    else if (right_child != NULL) {
                        btree_append_adjacent_node(target_node, right_child, node->keys[key_r.right_boundary]);

                        free_node(right_child);

                        // shrink this node using the empty place where node->keys[key_r.right_boundary] was
                        for (i=key_r.right_boundary; i < node->n - 1; i++) {
                            node->keys[i] = node->keys[i+1];
                            node->children[i+1] = node->children[i+2];
                        }
                        node->n = node->n - 1;
                    }
                    else {
                        die("btree_remove_node: both left and right children for the key do not exist, btree implementation error");
                    }

                    if (node->root == true && node->n == 0) {
                        tree->height = tree->height - 1;
                        tree->root = target_node;
                        target_node->root = true;
                        free_node(node);
                    }
                }
            }

            return btree_remove_node(tree, target_node, key);
        }

        return false;
    }

    return false;
}

// this is a main function to delete a key from btree
//
boolean btree_remove(btree *tree, BTREE_KEY_TYPE key) {
    btree_search_result *presult;
    btree_search_result result;

    presult = btree_search(tree, key, &result);
    if (presult == NULL) {
        printf("key [%d] doesn't exist in the tree, unable to remove\n", key);
        return false;
    }

    return btree_remove_node(tree, tree->root, key);
}

// find a key in the btree starting with the given node using binary search
//
btree_search_result *btree_search_node(btree_node *node, BTREE_KEY_TYPE key, btree_search_result *presult) {
    int left_boundary = 0;
    int right_boundary = BTREE_MAX_KEYS_IN_NODE - 1;
    int center;

    boolean found_key = false;
    boolean found_key_position;
    boolean done_searching = false;

    while(done_searching != true) {
        if (left_boundary >= node->n) {
            if (node->leaf == true) {
                done_searching = true;
                continue;
            }
            else {
                if (left_boundary > 0) {
                    // left boundary moved to center + 1 on the previous cycle,
                    // but it's not set (while previous center was), since it's
                    // not the leaf - descend into previous center "right" child
                    return btree_search_node(node->children[left_boundary], key, presult);
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

        if (center >= node->n) {
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
                    return btree_search_node(node->children[center], key, presult);
                }
                if (node->keys[center] < key) {
                    return btree_search_node(node->children[center + 1], key, presult);
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
        presult->found = true;
        presult->node = node;
        presult->matching_key_index = found_key_position;
        presult->key = node->keys[found_key_position];
        return presult;
    }
    else {
        return NULL;
    }
}

// takes btree and returns either NULL when key is not present in the btree
// or struct which holds the node pointer and index of the key
// in the result->node->keys array
//
btree_search_result *btree_search(btree *t, BTREE_KEY_TYPE key, btree_search_result *presult) {
    return btree_search_node(t->root, key, presult);
}

boolean btree_equal(btree *t1, btree *t2) {
    return true;
}

boolean btree_test() {
    btree t1;
    btree t2;

    btree_create(&t1, "actual");
    btree_create(&t2, "expected");

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
        die ("tests are not implemented yet");
        
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

    btree_create(&t, "tree 1");
    btree_dump(&t, false);

    char cmd[MAX_MSG_SIZE] = "";
    char cmd_short[MAX_MSG_SIZE] = "";
    BTREE_KEY_TYPE key;

    while (readcmd("[a]dd N, [r]emove N, [d]ump node storage, [sp]revious N, [sn]ext N, [q]uit: ", cmd)) {
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
                if (result.found == true) {
                    printf("found previous key [%d] for the key [%d] in node [%d]!\n",
                        result.key, key, result.node->id);
                }
                else {
                    printf("no previous key found for key [%d] in the tree!\n", key);
                }

//                btree_dump(&t, true);
//                continue;
            }
            if (strcmp(cmd_short, "sn") == 0) {
                result = btree_find_next(t.root, key);
                if (result.found == true) {
                    printf("found next key [%d] for the key [%d] in node [%d]!\n",
                        result.key, key, result.node->id);
                }
                else {
                    printf("no next key found for key [%d] in the tree!\n", key);
                }

//                btree_dump(&t, true);
//                continue;
            }
        }

        btree_dump(&t, false);
    }
    
    return 0;
}
