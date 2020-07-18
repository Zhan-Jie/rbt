#ifndef RBT_H
#define RBT_H

#define color_t char
#define RED 'r'
#define BLACK 'b'
#define new(type) (type*)malloc(sizeof(type))

typedef int (*comparator)(void*, void*);

typedef struct node {
    color_t color;
    void* key;
    struct node* left;
    struct node* right;
}node_t;

typedef struct rbt {
    comparator cmp;
    node_t* root;
} rbt_t;

/**
 * 插入值
 */ 
void rbt_insert(rbt_t* tree, void* key);

/**
 * 删除值
 */ 
void rbt_remove(rbt_t* tree, void* key);

/**
 * 删除整个树
 */ 
void rbt_destory(rbt_t* tree);

#endif