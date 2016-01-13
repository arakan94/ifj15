/******** ial.c *********
 *
 * FIT VUT, IFJ 089
 * Author: Ivo Juracek, xjurac05
 * Summary: Algorithms of binary tree, quick sort and boyer-moore alg.
 *
 */

/* **** Editation notes **** */
/*
 * 15.11: Merged files into 1
 */

/* **** end **** */

/* Include libraries */
#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<malloc.h>
#include "ial.h"
#include "error_codes.h"

#define MAXCHAR 255


/* * * * * * * * * * * * * * * * * * * B M A * * * * * * * * * * * * * * * * * * * * * *
 *  Function returns bigger value.
 */
int max(int a, int b){
    return a > b ? a : b;
}


/* * * * * * * * * * * * * * * * * * * * B M A * * * * * * * * * * * * * * * * * * * * *
 *  Function returns smaller value.
 */
int min(int a, int b){
    return a < b ? a : b;
}

/* * * * * * * * * * * * * * * * * * * * B M A * * * * * * * * * * * * * * * * * * * * *
 *  Procedure computes array of jumps. It is first heuristics of BMA.
 */
void compute_jumps(char *p, int *jump, int len_p){
    int k = 0;

    //Adding integer values of characters
    for(k = 0; k < MAXCHAR; k++)
        jump[k] = len_p;
    //Change value of letter in jump to length of pattern - position in jump array - 1
    for(k = 0; k < len_p; k++)
        jump[(unsigned char) p[k]] = len_p - k - 1;
}

/* * * * * * * * * * * * * * * * * * * * * B M A * * * * * * * * * * * * * * * * * * * *
 * Second heuristics of BMA. It counts maximal possible jump, if is found same letter in text
 * and in pattern, CMJ jumps to last letter in pattern, which should be same as text.
 */
void compute_match_jump(char* p, int *match_jump, int len_p){
    //Initialize variables
    int k = 0;
    int q = 0;
    int qq = 0;
    int save[len_p + 1];

    for(int i = 0; i < len_p + 1; i++){
        //initialize backup and match_jump
        if(i == len_p - 1)
            save[i + 1] = 0;
        save[i] = 0;
        match_jump[i] = 0;
    }

    //Walk through pattern and set match_jump indexes to 2* pattern length - actual letter (the biggest jump)
    for(k = 0; k < len_p + 1; k++)
        match_jump[k] = 2 * len_p - k;

    //set k to patter length and q to k+1
    k = len_p;
    q = len_p + 1;

    //while is k positive
    while(k > 0){
        //Make backup on index k to q
        save[k] = q;
        while(q <= len_p && p[k-1] != p[q-1]){
            //While q is smaller than pattern and pattern on k-1 != pattern on q-1,
            //make match_jump smaller number between match_nump[q] and pattern length - k
            match_jump[q] = min(match_jump[q], len_p - k);
            q = save[q];
        }
        //Make indexes about 1 smaller
        k--;
        q--;
    }

    //in part of pattern make values in match_jump min between this and len_p + q - k
    for(k = 0; k < q + 1; k++)
        match_jump[k] = min(match_jump[k], len_p + q - k);

    //Load from backup value in save[q]
    qq = save[q];

    //While part of pattern is smaller than pattern
    while(q <= len_p ){
        while(q <= qq){
            //Chose min between this and qq-q+len_p, increment q
            match_jump[q] = min(match_jump[q], qq - q + len_p);
            q++;
        }
        //Load from save
        qq = save[qq];
    }
}

/* * * * * * * * * * * * * * * * * * * * * B M A * * * * * * * * * * * * * * * * * * * *
 * Algorithm to find if pattern is in the string. If is found same letter, move left.
 */

int bma(char *p, char *t, int len_p, int len_t){
    int j = len_p;
    int char_jump[MAXCHAR];
    int match_jump[len_p];

    //Compute character jumps
    compute_jumps(p, char_jump, len_p);
    //Compute jumps if is pattern matched
    compute_match_jump(p, match_jump, len_p);

    //do jumps while actual position is in text and position of pattern > 0
    while(j <= len_t && len_p > 0){
        //if pattern and text is same, jump 1 position back
        if( t[j-1] == p[len_p-1]){

            j--;
            len_p--;
        }
        else {
            //not same, so jump about bigger value in char_jump or match_jump
            j += max(char_jump[(unsigned char)t[j]], match_jump[len_p]);
            //reset to left shifts
            len_p = strlen(p);
        }
    }
    //if is left shift 0, return position
    if (len_p == 0)
        return j + 1;
        else
        return len_t + 1;
}

/* * * * * * * * * * * * * * * Q U I C K * * S O R T * * * * * * * * * * * * * * * * * *
 * Procedure sorts string "text" ascending by character ASCII code. Sort algorithm is
 * QuickSort. It uses middle element and sort halves.
 */

void quicksort(char text[], int left, int right){
    //Find the middle element
    unsigned char middle = (unsigned char)text[(left + right) / 2];
    //Index i is left border, j is right border, temporary string for swap inc.
    int i = left;
    int j = right;
    unsigned char tmp = '\0';
    do {
        //Values smaller than middle value
        while((unsigned char)text[i] < middle)
            //Shift right i index (increment)
            i++;
        //Values greater than middle value
        while((unsigned char)text[j] > middle)
            //Shift left J index (decrement)
            j--;
        //If i index is smaller or equal than j
        if(i <= j){
            //Swap values
            tmp = (unsigned char)text[i];
            text[i] = (unsigned char)text[j];
            text[j] = tmp;
            //Change indexes
            i++;
            j--;
        }
    }
    //repeat until i < j
    while(i < j);
    //if j index is greater than left border, call sort with new borders
    if(j > left) quicksort(text, left, j);
    //if i index is smaller than right border, call sort with new borders
    if(i < right) quicksort(text, i, right);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function returns created binary tree. In function is allocated memory for new root.
 * Root pointers are NULLed and is inserted into new binary tree.
 */

TBinTree *initialize_tree(){
    TBinTree *bin_tree = malloc(sizeof(TBinTree));
    if(bin_tree != NULL)
        bin_tree->root = NULL; //Null binary tree root
    return bin_tree;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Procedure deletes branch. If node has left child, call this procedure with left-child
 * address, then do same to right child. Deallocate memory.
 */

static void destroy_branch(TSymTablePointer node){
    if(node->left_child != NULL){
        //Destroy left child and its children
        destroy_branch(node->left_child);
    }
    if(node->right_child != NULL){
        //Destroy right child and its children
        destroy_branch(node->right_child);
    }
    //Deallocate node
    free(node->key);
    free(node);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * In procedure is created copy of the roots pointer and then is called function
 * destroy_branch() with roots children. If is everything free, the root has been
 * destroyed like the tree.
 */

void delete_tree(TBinTree *tree){
    if(tree->root != NULL){
        destroy_branch(tree->root);
        tree->root = NULL;
        free(tree);
    }
    else {free(tree);}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * In procedure is created copy of the node pointer and then is called function
 * destroy_branch() with roots children. If is everything free, node has been destroyed.
 */

void delete_branch(TSymTablePointer node, TSymTablePointer tree){
    //Exist node
    if(node != NULL){
        //Initialize
        TSymTablePointer parent = NULL;
        TSymTablePointer searched_node = NULL;
        //find parent
        find_parent(tree, node->key, &searched_node, &parent);
        if(parent != NULL){
            //Parent found, remove ptr
            if(parent->left_child == node)
                parent->left_child = NULL;
            if(parent->right_child == node)
                parent->right_child = NULL;
            //Destroy
            destroy_branch(node);
        }
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Procedure searches position of the searched node. If is node null, tree is empty.
 * If not, try find node by key in tree. Respects binary tree rules.
 */

void search_insert(TSymTablePointer node, TKey key, bool *found, TSymTablePointer *pos){
    if(node == NULL){
        // Tree is empty
        *found = false;
        *pos = NULL;
    }
    else do{
        //Find the right position
        int compare = strcmp(key, node->key);
        *pos = node;
        if(compare < 0)
            // Visit left child
            node = node->left_child;
        else if(compare > 0)
            //Visit right child
            node = node->right_child;
        else
            //Node found
            *found = true;
    } while(*found == false && node != NULL);
        //while: node is not terminal and is not found yet
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * In empty tree, new node is allocated and set to values from parameters. Children
 * are NULL. If tree is not empty, pointer replaces itself with pointers to child,
 * which respect the rules of the binary tree (smaller than node insert
 * on the left side, bigger on the right side). Function return 0, if malloc is successful,
 * or returns ERR_INTERPRET, if malloc is not done correctly.
 */

int insert_into_tree(TSymTablePointer *node, TKey key, void *data){
    // Initialize temp pointer and position, where to insert new node
   TSymTablePointer temp_ptr = NULL;
   TSymTablePointer pos = NULL;
   bool found = false;
   bool malloc_error = false;

    // Find eight position
   search_insert(*node, key, &found, &pos);
   if(found == true)
        //Node exists, replace new data
        pos->data_ptr = data;
   else
        if((temp_ptr = malloc(sizeof(TSymTable)))){
            //malloc successful, initialize
            temp_ptr->left_child = NULL;
            temp_ptr->right_child = NULL;
            temp_ptr->data_ptr = data;
            temp_ptr->key = malloc(strlen(key)+1);
            if(temp_ptr->key != NULL)
                strcpy(temp_ptr->key, key);
        }
        else {malloc_error = true;} //ERROR: MALLOC
    // Empty position, insert to temp_ptr
    if(malloc_error != true){
        if(pos == NULL)
            *node = temp_ptr;
        else if(strcmp(key, pos->key) < 0)
            //insert to left child
                pos->left_child = temp_ptr;
            else pos->right_child = temp_ptr;
            return 0;
    }
    else return ERR_INTERPRET;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * If is tree empty, searched_mode is set to NULL, else if tmp_ptr->key is not my
 * finding key, then test this function with child in function parameter. Respecting
 * binary tree rules. If is found, the searched_node is set to this node.
 */

void search_in_tree(TSymTablePointer tmp_ptr, TKey key, TSymTablePointer *searched_node){
    if(tmp_ptr == NULL)
        //Tree is empty
        searched_node = NULL;
    else{
        while(tmp_ptr != NULL){
            int compare = strcmp(key, tmp_ptr->key);
            if(compare != 0)
                //Searched key is not equal to node key
                if(compare < 0)
                    //Search on the left
                    tmp_ptr = tmp_ptr->left_child;
                else
                    //Search on the right
                    tmp_ptr = tmp_ptr->right_child;
            else{
                    //Found, set into searched_node address
                    *searched_node = tmp_ptr;
                    tmp_ptr = NULL;
                }
        }
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Procedure finds node and it parent in the tree. Procedure is similar to
 * search_in_tree(), but parent node is returned too.
 */

void find_parent(TSymTablePointer tmp_ptr,
                 TKey key,
                 TSymTablePointer *searched_node,
                 TSymTablePointer *parent_node
                 ){
    if(tmp_ptr == NULL)
        //Tree is empty
        searched_node = NULL;
    else{
        while(tmp_ptr != NULL){
            int compare = strcmp(key, tmp_ptr->key);
            if(compare != 0)
                //Searched key is not equal to node key
                if(compare < 0){
                    //Search on the left
                    *parent_node = tmp_ptr;
                    tmp_ptr = tmp_ptr->left_child;}
                else{
                    //Search on the right
                    *parent_node = tmp_ptr;
                    tmp_ptr = tmp_ptr->right_child;}
            else{
                    //Save address
                    *searched_node = tmp_ptr;
                    tmp_ptr = NULL;
                }
        }
    }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Procedure calls find_parent() procedure to find nodes parent. It can not remove
 * the non existing node. If is node terminal, its parent's pointer is NULLed,
 * else parent's pointer is changed to nodes child. Respecting binary tree rules.
 */

void delete_node(TSymTablePointer tmp_ptr, TKey key){
    //Initialize: parent (NULL) and searched_node (NULL)
    TSymTablePointer parent = NULL;
    TSymTablePointer searched_node = NULL;

    //Try find parent and its child
    find_parent(tmp_ptr, key, &searched_node, &parent);
    if(searched_node == NULL){
        //node not found;
    }else if(searched_node->left_child == NULL && searched_node->right_child == NULL){
        //node is terminal
        if(parent->left_child == searched_node)
            //parent's left child is victim
            parent->left_child = NULL;
        else
            //parent's right child is victim
            parent->right_child = NULL;
        free(searched_node->key);
        free(searched_node);
    }
     //This part is not necessary, deletes non-terminal, 1 child node.
   /* else if((searched_node->left_child != NULL) != (searched_node->right_child != NULL)){
            //node is not terminal
            if(parent->left_child == searched_node)
                //parent's left child is victim
                parent->left_child = searched_node->left_child == NULL ? searched_node->right_child : searched_node->left_child;
            else
                //parent's right child is victim
                parent->right_child = searched_node->right_child == NULL ? searched_node->left_child : searched_node->right_child;
            free(searched_node->key);
            free(searched_node);
    }*/
    else{
        //NODE HAS 2 CHILDREN! NOT NECESSARY
    }
}
