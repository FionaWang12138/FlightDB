
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "List.h"
#include "Record.h"
#include "AVLTree.h"

typedef struct node *Node;
struct node {
    Record rec;
    Node   left;
    Node   right;
    int    height;
};

struct tree {
    Node    root;
    int     (*compare)(Record, Record);
};

////////////////////////////////////////////////////////////////////////
// Auxiliary functions

static void doTreeFree(Node n, bool freeRecords);
static Node newNode(Record rec);
static Record doTreeSearch(Tree t, Node n, Record rec);
static Node doTreeInsert(Tree t, Node n, Record rec, bool *res);
static int findHeight (Node n);
static int findMaxHeight (Node n1, Node n2);
static Node rightRotate(Node n);
static Node leftRotate(Node n);
static void doTreeSearchBetween(Tree t, Node n, Record lower,
                                Record upper, List l);



////////////////////////////////////////////////////////////////////////

static Node newNode(Record rec) {
    Node n = malloc(sizeof(*n));
    if (n == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    n->rec = rec;
    n->left = NULL;
    n->right = NULL;
    n->height = 0;
    return n;
}

////////////////////////////////////////////////////////////////////////

Tree TreeNew(int (*compare)(Record, Record)) {
    Tree t = malloc(sizeof(*t));
    if (t == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    t->root = NULL;
    t->compare = compare;
    return t;
}

////////////////////////////////////////////////////////////////////////

void TreeFree(Tree t, bool freeRecords) {
    doTreeFree(t->root, freeRecords);
    free(t);
}

static void doTreeFree(Node n, bool freeRecords) {
    if (n != NULL) {
        doTreeFree(n->left, freeRecords);
        doTreeFree(n->right, freeRecords);
        if (freeRecords) {
            RecordFree(n->rec);
        }
        free(n);
    }
}

////////////////////////////////////////////////////////////////////////

Record TreeSearch(Tree t, Record rec) {
    return doTreeSearch(t, t->root, rec);
}

static Record doTreeSearch(Tree t, Node n, Record rec) {
    if (n == NULL) {
        return NULL;
    }

    int cmp = t->compare(rec, n->rec);
    if (cmp < 0) {
        return doTreeSearch(t, n->left, rec);
    } else if (cmp > 0) {
        return doTreeSearch(t, n->right, rec);
    } else {
        return n->rec;
    }
}


////////////////////////////////////////////////////////////////////////
/* IMPORTANT: 
   Do NOT modify the code above this line. 
   You must not modify the 'node' and 'tree' structures defined above.
   You must not modify the functions defined above.
*/
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////

bool TreeInsert(Tree t, Record rec) {
    bool res = false; // if the record was inserted
    t->root = doTreeInsert(t, t->root, rec, &res);
    return res;
}


static Node doTreeInsert(Tree t, Node n, Record rec, bool *res) {
    
    if (n == NULL){
        //base case
        *res = true;
        return newNode(rec);
    }

    int cmp = t->compare(rec, n->rec);

    if (cmp < 0) {
        n->left = doTreeInsert(t, n->left, rec, res);
    } else if (cmp > 0) {
        n->right = doTreeInsert(t, n->right, rec, res);
    }
    //if the record already exist, cannot insert
    else {
        *res = false;
        return n;
    }
    //update height after inserting
    n->height = findMaxHeight(n->left,n->right) + 1;
    
    int hL = findHeight(n->left);
    int hR = findHeight(n->right);
    //Rotating to fix inbalances
    if ((hL - hR) > 1){
        if (t->compare(rec, n->left->rec) > 0) {
            n->left = leftRotate(n->left);
        }
        n = rightRotate(n);
    }
    else if ((hR - hL) > 1) {
        if (t->compare(rec, n->right->rec) < 0) {
            n->right = rightRotate(n->right);
        }
        n = leftRotate(n);
    }
    
    return n;
    
}

//helper functino to find heigh of a given node
static int findHeight (Node n) {
    //empty node height -1
    if (n==NULL) {
        return -1;
    } else {
        return n->height;
    }
}

//helper function to find the maximum height
static int findMaxHeight (Node n1, Node n2) {
    int height1 = findHeight(n1);
    int height2 = findHeight(n2);
    if (height1 > height2) {
        return height1;
    }
    else {
        return height2;
    }
}

static Node rightRotate(Node n) {
    Node n2 = n->left;
    Node temp = n2->right;
 
    // Perform rotation
    n2->right = n;
    n->left = temp;

    //Update height   
    n->height = findMaxHeight(n->left,n->right) + 1;
    n2->height = findMaxHeight(n2->left,n2->right) + 1;

    // Return new root
    return n2;
}

static Node leftRotate(Node n){
    Node n2 = n->right;
    Node temp = n2->left;
 
    // Perform rotation
    n2->left = n;
    n->right = temp;
 
    // Update heights
    n->height = findMaxHeight(n->left,n->right) + 1;
    n2->height = findMaxHeight(n2->left,n2->right) + 1;

    // Return new root
    return n2;
}

////////////////////////////////////////////////////////////////////////

List TreeSearchBetween(Tree t, Record lower, Record upper) {
    List l = ListNew();
    
    doTreeSearchBetween (t, t->root, lower, upper, l);
    return l;
}

static void doTreeSearchBetween(Tree t, Node n, Record lower,
                                Record upper, List l) {

    if (n == NULL || n->rec == NULL) {
        return;
    }
    //only search in left subtree if current node is larger than the lower bound
    if (t->compare(lower,n->rec) < 0) {
        doTreeSearchBetween (t, n->left, lower, upper, l);
    }
    if (t->compare(lower,n->rec) <= 0 && t->compare(upper,n->rec) >= 0){
        ListAppend(l, n->rec);
    }
    //only search in the right subtree if the current node is smaller than the upper bound
    if (t->compare(upper, n->rec) > 0) {
        doTreeSearchBetween (t, n->right, lower, upper, l);
    }
    
}

////////////////////////////////////////////////////////////////////////
//looks for the next largest record
Record TreeNext(Tree t, Record r) {
   
    Node prev = NULL;
    Node current = t->root;
    while (current != NULL) {
        //if the current record is larger than r, we look to the left tree
        if (t->compare(r, current->rec)< 0) {
            prev = current;
            current = current->left;
            //if there is nothing in the new current record, or if everything in the new current sub-tree is
            //smaller than r, the previous record must be the next largest record after r.
            if (current == NULL || (t->compare(r,current->rec)>0 && current->right == NULL)) {
                return prev->rec;
            }
        }
        else if (t->compare(r,current->rec)> 0){
            //if current record is smaller than r, we look in the right subtree
            //don't move prev, becuase prev always points at a record bigger than r
            current = current->right;
        }
        else
            //if current record is exactly same as r, return current record
            return current->rec;
    }
    if (prev != NULL) {
        return prev->rec;
    }
    //in the case where the current node is the biggest, return NULL
    else{
        return NULL;
    }
}

