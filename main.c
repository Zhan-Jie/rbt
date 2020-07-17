#include <stdio.h>
#include <stdlib.h>

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

int is_red(node_t* h) {
    return h && h->color == RED;
}

node_t* rotate_left(node_t* h) {
    node_t* r = h->right;
    h->right = r->left;
    r->left = h;
    color_t r_color = r->color;
    r->color = h->color;
    h->color = r_color;
    return r;
}

node_t* rotate_right(node_t* h) {
    node_t* l = h->left;
    h->left = l->right;
    l->right = h;
    color_t l_color = l->color;
    l->color = h->color;
    h->color = l_color;
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

node_t* find_successor(node_t* root) {
    node_t* t = root->right;
    
    while(t->left) {
        t = t->left;
    }
    return t;
}

node_t* find_in_tree(node_t* root, comparator cmp, void* key) {
    if (!root) {
        return NULL;
    }
    int c = cmp(key, root->key);
    if (c == 0) {
        return root;
    }
    if (c < 0) {
        return find_in_tree(root->left, cmp, key);
    }
    return find_in_tree(root->right, cmp, key);
}

node_t* find_parent_in_tree(node_t* root, comparator cmp, node_t* target) {
    if (!root) {
        return NULL;
    }
    int c = cmp(target->key, root->key);
    if (c < 0) {
        if (root->left == target) {
            return root;
        }
        return find_parent_in_tree(root->left, cmp, target);
    } 
    if (c > 0) {
        if (root->right == target) {
            return root;
        }
        return find_parent_in_tree(root->right, cmp, target);
    }
    return NULL;
}

node_t* delete_black_leaf(node_t* h, comparator cmp, node_t* parent, int is_left) {
    if (is_left) {
        node_t* sibling = parent->right;

        if (parent->color == RED) {
            // 父节点为红色，则直接父节点涂黑，右节点涂红
            parent->color = BLACK;
            sibling->color = RED;
            return h;
        }

        if (sibling->color == RED) {
            // 右兄弟为红色，则右兄弟两子为黑节点
            // P节点左旋，导致P节点涂红，S节点变成新的父节点
            node_t* new_parent = rotate_left(parent);
            new_parent->left->color = BLACK;
            new_parent->left->right->color = RED;
            return parent == h ? new_parent : h;
        }
        // 右兄弟为黑色
        if (is_red(sibling->right)) {
            // 右兄弟的右儿子红色
            node_t* new_parent = rotate_left(parent);
            new_parent->right->color = BLACK;
            return parent == h ? new_parent : h;
        } 
        if (is_red(sibling->left)) {
            // 右兄弟的左儿子红色
            parent->right = rotate_right(parent->right);
            node_t* new_parent = rotate_left(parent);
            new_parent->right->color = BLACK;
            return parent == h ? new_parent : h;
        } 
        // 右兄弟黑色，无红色子节点
        sibling->color = RED;
        // 递归向上时，仅仅平衡高度，不能删除节点
        node_t* new_parent = find_parent_in_tree(h, cmp, parent);
        if (new_parent == NULL) {
            return h;
        }
        return delete_black_leaf(h, cmp, new_parent, new_parent->left == parent);
    } else {
        
        node_t* sibling = parent->left;
        
        if (parent->color == RED) {
            // 父节点为红色，则直接父节点涂黑，左节点涂红
            parent->color = BLACK;
            sibling->color = RED;
            return h;
        }

        if (sibling->color == RED) {
            node_t* new_parent = rotate_right(parent);
            new_parent->right->color = BLACK;
            new_parent->right->left->color = RED;
            return parent == h ? new_parent : h;
        }

        if (is_red(sibling->left)) {
            node_t* new_parent = rotate_right(parent);
            new_parent->left->color = BLACK;
            return parent == h ? new_parent : h;
        }

        if (is_red(sibling->right)) {
            parent->left = rotate_left(parent->left);
            node_t* new_parent = rotate_right(parent);
            new_parent->left->color = BLACK;
            return parent == h ? new_parent : h;
        }
        sibling->color = RED;
        // 递归向上时，仅仅平衡高度，不能删除节点
        node_t* new_parent = find_parent_in_tree(h, cmp, parent);
        if (new_parent == NULL) {
            return h;
        }
        return delete_black_leaf(h, cmp, new_parent, new_parent->left == parent);
    }
}

node_t* delete_node(node_t* h, comparator cmp, void* key) {
    node_t* target = find_in_tree(h, cmp, key);
    node_t* parent = NULL;
    node_t* successor = NULL;
    if (!target) {
        return h;
    }
    if (target->left) {
        if (target->right) {
            // 左右均不为空，则找到直接后继节点（叶子节点）
            successor = find_successor(target);
            if (successor->right) {
                // successor左null右红.
                target->key = successor->key;
                successor->key = successor->right->key;
                free(successor->right);
                successor->right = NULL;
                return h;
            }
            // successor两子为null
            if (successor->color == RED) {
                // successor是红节点，可以代替target删掉
                node_t* parent = find_parent_in_tree(h, cmp, successor);
                target->key = successor->key;
                if (parent->left == successor) {
                    free(parent->left);
                    parent->left = NULL;
                } else {
                    free(parent->right);
                    parent->right = NULL;
                }
                return h;
            }
            void* successorKey = successor->key;
            node_t* new_parent = find_parent_in_tree(h, cmp, successor);
            if (new_parent == NULL) {
                return h;
            }
            int is_left = (new_parent->left == successor);
            free(successor);
            if (is_left) {
                new_parent->left = NULL;
            } else {
                new_parent->right = NULL;
            }
            // successor为黑节点，并且两子为null
            h = delete_black_leaf(h, cmp, new_parent, is_left);
            target->key = successorKey;
            return h;
        } else {
            // target左红右null
            target->key = target->left->key;
            free(target->left);
            target->left = NULL;
            return h;
        }
    } else {
        if (target->right) {
            // target左null右红
            target->key = target->right->key;
            free(target->right);
            target->right = NULL;
            return h;
        } 
        // target两子都为null
        if (target->color == RED) {
            // 红色叶子，左子为null，右子必为null，可直接删除不影响平衡
            parent = find_parent_in_tree(h, cmp, target);
            if (parent->left == target) {
                parent->left = NULL;
                free(target);
            } else {
                parent->right = NULL;
                free(target);
            }
            return h;
        } 
        // target为黑节点，两子为null
        node_t* new_parent = find_parent_in_tree(h, cmp, target);
        if (new_parent == NULL) {
            free(target);
            return NULL;
        }
        int is_left = (new_parent->left == target);
        free(target);
        if (is_left) {
            new_parent->left = NULL;
        } else {
            new_parent->right = NULL;
        }

        return delete_black_leaf(h, cmp, new_parent, is_left);
    }
}

void tree_insert(rbt_t* tree, void* key) {
    tree->root = insert_new_node(tree->root, tree->cmp, key);
    tree->root->color = BLACK;
}

void tree_delete(rbt_t* tree, void* key) {
    tree->root = delete_node(tree->root, tree->cmp, key);
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
    char a[9] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    for (int i = 0; i < 9; ++i) {
        tree_insert(&tree, a+i);
    }
    
    tree_delete(&tree, a+1);
    tree_delete(&tree, a+5);
    tree_delete(&tree, a+3);
    tree_delete(&tree, a+7);
    tree_delete(&tree, a+4);
    tree_delete(&tree, a+8);
    tree_delete(&tree, a+2);
    tree_delete(&tree, a+6);
    show_tree(tree.root);
    return 0;
}