#include <stdlib.h>

#include "rbt.h"

static int is_red(node_t* h) {
    return h && h->color == RED;
}

static node_t* rotate_left(node_t* h) {
    node_t* r = h->right;
    h->right = r->left;
    r->left = h;
    color_t r_color = r->color;
    r->color = h->color;
    h->color = r_color;
    return r;
}

static node_t* rotate_right(node_t* h) {
    node_t* l = h->left;
    h->left = l->right;
    l->right = h;
    color_t l_color = l->color;
    l->color = h->color;
    h->color = l_color;
    return l;
}

static node_t* insert_new_node(node_t* h, comparator cmp, void* key) {
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

static node_t* find_successor(node_t* root) {
    node_t* t = root->right;
    
    while(t->left) {
        t = t->left;
    }
    return t;
}

static node_t* find_in_tree(node_t* root, comparator cmp, void* key) {
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

static node_t* find_parent_in_tree(node_t* root, comparator cmp, node_t* target) {
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

static color_t get_color(node_t* node) {
    return node == NULL ? BLACK : node->color;
}

static node_t* rebalance(node_t* h, comparator cmp, node_t* parent, int is_left) {
    if (parent == NULL) {
        return h;
    }
    if (is_left) {
        node_t* sibling = parent->right;
        node_t* sl = sibling->left;
        node_t* sr = sibling->right;

        if (get_color(sibling) == RED) {
            // 兄弟节点为红色，通过左旋，转化为兄弟节点为黑色的场景
            node_t* gp = find_parent_in_tree(h, cmp, parent);
            if (gp != NULL) {
                if (gp -> left == parent) {
                    gp -> left = sibling;
                } else {
                    gp -> right = sibling;
                }
            } 
            parent -> right = sl;
            sibling -> left = parent;

            sibling -> color = BLACK;
            parent -> color = RED;

            if (gp == NULL) {
                h = sibling;
            }

            sibling = sl;
            sl = sibling -> left;
            sr = sibling -> right;
        }
        
        // 兄弟是黑色的
        if (get_color(sl) == BLACK && get_color(sr) == BLACK) {
            // 两兄弟子节点黑色
            if (get_color(parent) == BLACK) {
                // 父节点是黑色的
                sibling->color = RED;
                node_t* gp = find_parent_in_tree(h, cmp, parent);
                return rebalance(h, cmp, gp, gp != NULL && gp -> left == parent);
            } else {
                // 父节点是红色的
                parent->color = BLACK;
                sibling->color = RED;
                return h;
            }
        } 
        if (get_color(sr) == BLACK) {
            // 兄弟右子为黑色，通过右旋，转化为兄弟右子为红色的情况
            parent -> right = sl;
            node_t* slr = sl -> right;
            sl-> right = sibling;
            sibling -> left = slr;
            sibling -> color = RED;
            sl -> color = BLACK;

            sibling = sl;
            sl = sibling -> left;
            sr = sibling -> right;
        }
        // 兄弟右子节点为红色
        node_t* gp = find_parent_in_tree(h, cmp, parent);
        if (gp != NULL) {
            if (gp -> left == parent) {
                gp -> left = sibling;
            } else {
                gp -> right = sibling;
            }
        } 
        color_t parent_color = get_color(parent);
        parent -> right = sl;
        sibling -> left = parent;
        sibling -> color = parent_color;
        parent -> color = BLACK;
        sr -> color = BLACK;
        return gp != NULL ? h : sibling;
    } else {
        node_t* sibling = parent->left;
        node_t* sl = sibling -> left;
        node_t* sr = sibling -> right;

        // 兄弟节点为红色
        if (get_color(sibling) == RED) {
            // 通过右旋，转化为兄弟节点为黑色的情况
            node_t* gp = find_parent_in_tree(h, cmp, parent);
            if (gp != NULL) {
                if (gp -> left == parent) {
                    gp -> left = sibling;
                } else {
                    gp -> right = sibling;
                }
            }

            sibling -> right = parent;
            parent -> left = sr;

            sibling -> color = BLACK;
            parent -> color = RED;

            if (gp == NULL) {
                h = sibling;
            }

            sibling = sr;
            sl = sibling -> left;
            sr = sibling -> right;
        }

        // 兄弟节点是黑色的
        if (get_color(sl) == BLACK && get_color(sr) == BLACK) {
            // 兄弟的两子节点为黑色
            if (get_color(parent) == BLACK) {
                // 父节点是黑色的
                sibling -> color = RED;
                node_t* gp = find_parent_in_tree(h, cmp, parent);
                return rebalance(h, cmp, gp, gp != NULL && gp -> left == parent);
            } else {
                parent -> color = BLACK;
                sibling -> color = RED;
                return h;
            }
        }
        
        if (get_color(sl) == BLACK) {
            // 兄弟左子节点为黑色的情况，通过左旋，转化为兄弟左子节点为红色的情况
            parent -> left = sr;
            node_t* srl = sr -> left;
            sr -> left = sibling;
            sibling -> right = srl;
            sibling -> color = RED;
            sr -> color = BLACK;

            sibling = sr;
            sl = sibling -> left;
            sr = sibling -> right;
        }

        // 兄弟的左子节点，为红色
        node_t* gp = find_parent_in_tree(h, cmp, parent);
        if (gp != NULL) {
            if (gp -> left == parent) {
                gp -> left = sibling;
            } else {
                gp -> right = sibling;
            }
        }
        color_t parent_color = get_color(parent);
        parent -> left = sr;
        sibling -> right = parent;
        sl -> color = BLACK;
        parent -> color = BLACK;
        sibling -> color = parent_color;

        return gp != NULL ? h : sibling;
    }
}

static node_t* delete_node(node_t* h, comparator cmp, void* key) {
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
            if (successor->right != NULL) {
                // successor左null，右边不为空，则必定是红色节点. 直接删除即可
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
            // successor为黑节点，且无子节点
            void* successorKey = successor->key;
            node_t* new_parent = find_parent_in_tree(h, cmp, successor);
            int is_left = (new_parent->left == successor);
            free(successor);
            target->key = successorKey;
            if (is_left) {
                new_parent->left = NULL;
            } else {
                new_parent->right = NULL;
            }
            // successor为黑节点，并且两子为null
            return rebalance(h, cmp, new_parent, is_left);
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

        return rebalance(h, cmp, new_parent, is_left);
    }
}

/**
 * 插入节点
 */ 
void rbt_insert(rbt_t* tree, void* key) {
    tree->root = insert_new_node(tree->root, tree->cmp, key);
    tree->root->color = BLACK;
}

/**
 * 删除节点
 */
void rbt_remove(rbt_t* tree, void* key) {
    tree->root = delete_node(tree->root, tree->cmp, key);
}

static void delete_all(node_t* root) {
    if (!root) {
        return;
    }
    node_t* left = root->left;
    node_t* right = root->right;
    free(root);
    delete_all(left);
    delete_all(right);
}

/**
 * 清空红黑树
 */ 
void rbt_destory(rbt_t* tree) {
    delete_all(tree->root);
    tree->root = NULL;
}