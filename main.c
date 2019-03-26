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
    if (is_red(h->left)) {
        if (is_red(h->left->right)) {
            h->left = rotate_left(h->left);
        }
        if (is_red(h->left->left)) {
            h = rotate_right(h);
        }
    } else if (is_red(h->right)) {
        if (is_red(h->right->left)) {
            h->right = rotate_right(h->right);
        }
        if (is_red(h->right->right)) {
            h = rotate_left(h);
        }
    }
    return h;
}

void tree_insert(rbt_t* tree, void* key) {
    tree->root = insert_new_node(tree->root, tree->cmp, key);
    tree->root->color = BLACK;
}

char to_char(void* v) {
    return *((char *)v);
}

char* get_color(char c) {
    return c == RED ? "red" : "black";
}

void show_tree(node_t* h) {
    if (h == NULL) {
        return;
    }
    if (h->color == RED) {
        printf("%c [style=filled, fillcolor=%s]\n", to_char(h->key), get_color(h->color));
    } else {
        printf("%c\n", to_char(h->key));
    }
    if (h->left) {
        printf("%c -- %c\n", to_char(h->key), to_char(h->left->key));
    }
    if (h->right) {
        printf("%c -- %c\n", to_char(h->key), to_char(h->right->key));
    }
    show_tree(h->left);
    show_tree(h->right);
    free(h);
}

int cmp_char(void* a, void* b) {
    return *((char *)a) - *((char *)b);
}

int main() {
    rbt_t tree;
    tree.root = NULL;
    tree.cmp = cmp_char;
    char a[10] = {'E', 'A', 'S', 'Y', 'Q', 'U', 'T', 'I', 'O', 'N'};
    for (int i = 0; i < 10; ++i) {
        tree_insert(&tree, a+i);
    }
    show_tree(tree.root);
    return 0;
}