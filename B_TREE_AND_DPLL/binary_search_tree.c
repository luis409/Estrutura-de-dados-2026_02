#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//
// Created by luisgabrielalencar on 12/10/2026.
//

typedef struct item {
    int key;
    void *data;
} item_t;

typedef struct node {
    struct node *left;
    struct node *right;
    struct node *parent;
    struct item *item;
} node_t;

typedef struct tree {
    struct node *root;
} tree_t;

item_t *initialize_item(int key, void *data, size_t size_type);
node_t *initialize_node(int key, void *data, size_t size_type);
tree_t *initialize_tree();

void inorder_tree_walk(node_t *x, void(*display_fuction)(item_t *item));
void preorder_tree_walk(node_t *x, void(*display_fuction)(item_t *item));
void postorder_tree_walk(node_t *x, void(*display_fuction)(item_t *item));

node_t *tree_search(node_t *x, int key);
node_t *tree_minimum(node_t *x);
node_t *tree_maximum(node_t *x);
node_t *tree_successor(node_t *x);

void tree_insert(tree_t *t, node_t *z);
void display_item(item_t *item);
void transplant(tree_t *T, node_t *u, node_t *v);
void tree_delete(tree_t *T, node_t *z);

int main() {
    int keys[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    char strings[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'z', '|'};
    tree_t *T = initialize_tree();

    for (int i = 0; i < 10; i++) {
        node_t *new_Node = initialize_node(keys[i], &strings[i], sizeof(char));
        tree_insert(T, new_Node);
    }
    inorder_tree_walk(T->root, display_item);
    printf("\tRemovendo Nó\n");
    tree_delete(T, T->root->right);
    inorder_tree_walk(T->root, display_item);
}

item_t *initialize_item(int key, void *data, size_t size_type) {
    item_t *node = (item_t *)malloc(sizeof(item_t));
    node->key = key;
    node->data = malloc(size_type);
    memcpy(node->data, data, sizeof(void*));
    return node;
}
node_t *initialize_node(int key, void *data, size_t size_type) {
    item_t *item = initialize_item(key, data, size_type);
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->item = item;
    return node;
}
tree_t *initialize_tree() {
    tree_t *tree = (tree_t *)malloc(sizeof(tree_t));
    tree->root = NULL;
    return tree;
}

void inorder_tree_walk(node_t *x, void(*display_fuction)(item_t *item)) {
    if (x == NULL) { return; }
    inorder_tree_walk(x->left, display_fuction);
    display_item(x->item);
    inorder_tree_walk(x->right, display_fuction);
}
void preorder_tree_walk(node_t *x, void(*display_fuction)(item_t *item)) {
    if (x == NULL) { return; }
    display_item(x->item);
    preorder_tree_walk(x->left, display_fuction);
    preorder_tree_walk(x->right, display_fuction);
}
void postorder_tree_walk(node_t *x, void(*display_fuction)(item_t *item)) {
    if (x == NULL) { return; }
    postorder_tree_walk(x->left, display_fuction);
    postorder_tree_walk(x->right, display_fuction);
    display_item(x->item);
}
void display_item(item_t *item) {
    printf("\tkey: %d -> data: %c\n", item->key, *((char *)item->data));
}
node_t *tree_search(node_t *x, int key) {
    if (x == NULL || x->item->key == key) {
        return x;
    }
    if (x->item->key > key) { return tree_search(x->left, key); }
    return tree_search(x->right, key);
}
node_t *tree_minimum(node_t *x) {
    while (x->left != NULL) { x = x->left; }
    return x;
}
node_t *tree_maximum(node_t *x) {
    while (x->right != NULL) { x = x->right; }
    return x;
}
node_t *tree_successor(node_t *x) {
    if (x->right != NULL) { return tree_minimum(x->right); }
    node_t *y = x->parent;
    while (y != NULL && x == y->right) {
        x = y;
        y = y->parent;
    }
    return y;
}
void tree_insert(tree_t *T, node_t *z) {
    node_t *y = NULL;
    node_t *x = T->root;
    while (x != NULL) {
        y = x;
        if (x->item->key > z->item->key) { x = x->left; }
        else { x = x->right; }
    }
    z->parent = y;
    if (y == NULL) { T->root = z; }
    else if (z->item->key < y->item->key) { y->left = z; }
    else { y->right = z; }
}
void transplant(tree_t *T, node_t *u, node_t *v) {
    if (u->parent == NULL) {
        T->root = v;
    }else if (u == u->parent->left) {
        u->parent->left = v;
    }else u->parent->right = v;
    if (v != NULL) { v->parent = u->parent; }
}
void tree_delete(tree_t *T, node_t *z) {
    if (z == NULL) { return; }
    node_t *y = NULL;
    if (z->left == NULL) {
        transplant(T, z, z->right);
    }else if (z->right == NULL) {
        transplant(T, z, z->left);
    }else {
        y = tree_minimum(z->right);
        if (y->parent != z) {
            transplant(T, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        transplant(T, z, y);
        y->left = z->left;
        y->left->parent = y;
    }
}