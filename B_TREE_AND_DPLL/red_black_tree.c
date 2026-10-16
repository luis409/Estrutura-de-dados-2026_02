//
// Created by luisgabrielalencar on 12/10/2026.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLACK 0
#define RED   1
#define GRAY  2

typedef struct item {
    int key;
    void *data;
} item_t;

typedef struct node {
    struct node *left;
    struct node *right;
    struct node *parent;
    item_t *item;
    int color;
} node_t;

typedef struct tree {
    struct node *root;
    node_t *nil;
} tree_t;

item_t *initialize_item(int key, void *data, size_t size_type);
node_t *initialize_node(tree_t *T,int key, void *data, size_t size_type);
tree_t *initialize_tree();

void left_rotate(tree_t *T, node_t *x);
void right_rotate(tree_t *T, node_t *y);
void rb_insert(tree_t *T, node_t *z);
void rb_insert_fixup(tree_t *T, node_t *z);

void inorder(tree_t *T, node_t *x);
int main() {
    tree_t *T = initialize_tree();

    int keys[] = {10, 20, 30, 15, 25, 5, 1, 50, 40, 35};
    for (int i = 0; i < 10; i++) {
        node_t *z = initialize_node(T,keys[i], &keys[i], sizeof(int));
        rb_insert(T, z);
    }

    // In-order imprime em ordem crescente se a BST está correta
    printf("In-order: ");
    inorder(T, T->root);
    printf("\n");

    // Verifica propriedade da raiz
    printf("Raiz: %d (cor=%d, esperado BLACK=0)\n",
           T->root->item->key, T->root->color);

    return 0;
}

item_t *initialize_item(int key, void *data, size_t size_type) {
    item_t *item = (item_t *)malloc(sizeof(item_t));
    item->key = key;
    item->data = malloc(size_type);
    memcpy(item->data, data, size_type);
    return item;
}
node_t *initialize_node(tree_t *T, int key, void *data, size_t size_type) {
    item_t *item = initialize_item(key, data, size_type);
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->left   = T->nil;
    node->right  = T->nil;
    node->parent = T->nil;
    node->item   = item;
    node->color  = RED;
    return node;
}
tree_t *initialize_tree() {
    tree_t *tree = (tree_t *)malloc(sizeof(tree_t));
    tree->nil = (node_t *)malloc(sizeof(node_t));

    tree->nil->left   = tree->nil;   // ✅ aponta pra si mesmo
    tree->nil->right  = tree->nil;   // ✅
    tree->nil->parent = tree->nil;   // ✅
    tree->nil->item   = NULL;
    tree->nil->color  = BLACK;

    tree->root = tree->nil;          // ✅ raiz = nil (árvore vazia)
    return tree;
}

void left_rotate(tree_t *T, node_t *x) {
    node_t *y = x->right;

    // 1. B vira filho direito de x
    x->right = y->left;
    if (y->left != T->nil)
        y->left->parent = x;          // ✅ B->parent = x

    // 2. y sobe: herda o pai de x
    y->parent = x->parent;
    if (x->parent == T->nil)
        T->root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    // 3. x vira filho esquerdo de y (SÓ NO FINAL!)
    y->left = x;
    x->parent = y;
}
void right_rotate(tree_t *T, node_t *y) {
    node_t *x = y->left;

    y->left = x->right;
    if (x->right != T->nil) x->right->parent = y;

    x->parent = y->parent;
    if (y->parent == T->nil) T->root = x;
    else if (y == y->parent->left) y->parent->left = x;
    else y->parent->right = x;
    x->right = y;
    y->parent = x;
}

void rb_insert(tree_t *T, node_t *z) {
    node_t *y = T->nil;
    node_t *x = T->root;
    while (x != T->nil) {
        y = x;
        if (z->item->key < x->item->key) {
            x = x->left;
        }else x = x->right;
    }
    z->parent = y;
    if (y == T->nil) {
        T->root = z;
    }
    else if (z->item->key < y->item->key) {
        y->left = z;
    }else y->right = z;
    z->left = T->nil;
    z->right = T->nil;
    z->color = RED;
    rb_insert_fixup(T, z);
}
void rb_insert_fixup(tree_t *T, node_t *z) {
    node_t *y = NULL;
    while (z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            y = z->parent->parent->right;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z = z->parent->parent;
            }else {
                if (z == z->parent->right) {
                    z = z->parent;
                    left_rotate(T, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                right_rotate(T, z->parent->parent);
            }
        }else {
            y = z->parent->parent->left;    // tio

            if (y != NULL && y->color == RED) {
                z->parent->color         = BLACK;
                y->color                 = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    right_rotate(T, z);
                }
                z->parent->color         = BLACK;
                z->parent->parent->color = RED;
                left_rotate(T, z->parent->parent);
            }
        }
    }
    T->root->color = BLACK;
}
void inorder(tree_t *T, node_t *x) {
    if (x == T->nil) { return; }
    inorder(T, x->left);
    printf("\tkey: %d -> data: %c\n", x->item->key, *((char *)x->item->data));
    inorder(T, x->right);
}