/******** ial.h *********
 *
 * FIT VUT, IFJ 089
 * Author: Ivo Juracek, xjurac05
 * Summary: Algorithms of binary tree, quick sort and boyer-moore alg.
 *
 */

#ifndef IALFILES
#define IALFILES

typedef char* TKey;

typedef struct sym_table{
    struct sym_table* left_child;
    struct sym_table* right_child;
    void* data_ptr;
    TKey key;
}TSymTable;

typedef TSymTable *TSymTablePointer;

typedef struct binary_tree{
    TSymTablePointer root;
}TBinTree;


int bma(char *p, char *t, int len_p, int len_t);
void compute_jumps(char *p, int *jump, int len_p);
void compute_match_jump(char* p, int *match_jump, int len_p);
void delete_node(TSymTablePointer tmp_ptr, TKey key);
void delete_tree(TBinTree *tree);
void find_parent(TSymTablePointer tmp_ptr,
                 TKey key,
                 TSymTablePointer *searched_node,
                 TSymTablePointer *parent_node
                 );
TBinTree *initialize_tree();
int insert_into_tree(TSymTablePointer *node, TKey key, void *data);
int max(int a, int b);
int min(int a, int b);
void quicksort(char text[], int left, int right);
void search_in_tree(TSymTablePointer tmp_ptr, TKey key, TSymTablePointer *searched_node);
void search_insert(TSymTablePointer node, TKey key, bool *found, TSymTablePointer *pos);

#endif
