#include <stdio.h>
#include <stdlib.h>

#include "rbt.h"

char to_char(void* v) {
    return *((char *)v);
}

char* get_color(char c) {
    return c == RED ? "red" : "black";
}

void show_tree_node(node_t* h) {
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
    show_tree_node(h->left);
    show_tree_node(h->right);
}

void show_tree(rbt_t* tree) {
    printf("graph G {\n");
    show_tree_node(tree->root);
    printf("}\n");
}

int cmp_char(void* a, void* b) {
    return *((char *)a) - *((char *)b);
}

int main() {
    rbt_t tree;
    tree.root = NULL;
    tree.cmp = cmp_char;
    char a[9] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    for (int i = 0; i < 9; ++i) {
        rbt_insert(&tree, a+i);
    }
    rbt_remove(&tree, a+3);
    rbt_remove(&tree, a+1);
    rbt_remove(&tree, a+4);
    rbt_remove(&tree, a+2);
    rbt_remove(&tree, a+6);
    rbt_remove(&tree, a+5);
    rbt_remove(&tree, a);
    rbt_remove(&tree, a+7);
    rbt_remove(&tree, a+8);
    show_tree(&tree);
    rbt_destory(&tree);
    return 0;
}