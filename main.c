#include <stdio.h>
#include <stdlib.h>

#define RED 'r'
#define BLACK 'b'
#define new(type) (type*)malloc(sizeof(type))

typedef int (*comparator)(void*, void*);

typedef struct node {
    char color;
    void* key;
    struct node* left;
    struct node* right;
}node_t;

typedef struct rbt {
    comparator cmp;
    node_t* root;
} rbt_t;

int is_red(node_t* h) {
    return h && h->color == RED;
}

node_t* rotate_left(node_t* h) {
    node_t* r = h->right;
    h->right = r->left;
    r->left = h;
    r->color = h->color;
    h->color = RED;
    return r;
}

node_t* rotate_right(node_t* h) {
    node_t* l = h->left;
    h->left = l->right;
    l->right = h;
    l->color = h->color;
    h->color = RED;
    return l;
}

node_t* insert_new_node(node_t* h, comparator cmp, void* key) {
    if (h == NULL) {
        h = new(node_t);
        h->color = RED;
        h->key = key;
        h->left = h->right = NULL;
        return h;
    }
    if (is_red(h->left) && is_red(h->right)) {
        h->left->color = BLACK;
        h->right->color = BLACK;
        h->color = RED;
    }
    int c = cmp(key, h->key);
    if (c < 0) {
        h->left = insert_new_node(h->left, cmp, key);
    } else if (h > 0) {
        h->right = insert_new_node(h->right, cmp, key);
    } else {
        h->key = key;
    }
    if (is_red(h->right) && !is_red(h->left)) {
        h = rotate_left(h);
    }
    if (is_red(h->left) && is_red(h->left->left)) {
        h = rotate_right(h);
    }
    return h;
}

void tree_insert(rbt_t* tree, void* key) {
    tree->root = insert_new_node(tree->root, tree->cmp, key);
    tree->root->color = BLACK;
}

int to_int(void* v) {
    return *((int*)v);
}

char* get_color(char c) {
    return c == RED ? "red" : "black";
}

void show_tree(node_t* h) {
    if (h == NULL) {
        return;
    }
    if (h->color == RED) {
        printf("%d [style=filled, fillcolor=%s]\n", to_int(h->key), get_color(h->color));
    } else {
        printf("%d\n", to_int(h->key));
    }
    if (h->left) {
        printf("%d -- %d\n", to_int(h->key), to_int(h->left->key));
    }
    if (h->right) {
        printf("%d -- %d\n", to_int(h->key), to_int(h->right->key));
    }
    show_tree(h->left);
    show_tree(h->right);
    free(h);
}

int cmp_int(void* a, void* b) {
    return *((int*)a) - *((int*)b);
}

int main() {
    rbt_t tree;
    tree.root = NULL;
    tree.cmp = cmp_int;
    int a[20];
    for (int i = 0; i < 20; ++i) {
        a[i] = 20-i;
        tree_insert(&tree, a+i);
    }
    show_tree(tree.root);
    return 0;
}