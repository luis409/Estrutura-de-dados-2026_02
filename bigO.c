#include<stdio.h>
#include<stdlib.h>
#include<time.h>

typedef struct LL{
    int node;
    struct LL* next;
}LL;

typedef struct List{
    LL* head;
}List;

typedef struct Tn{
    int value;
    struct Tn* left;
    struct Tn* right;
}Tn;

typedef struct Tree{
    Tn* root;
}Tree;

int randomNumber(int limit){
    unsigned int result = ((unsigned int)(rand() & 32767) << 15) | (unsigned int)(rand() & 32767);
    return result % limit;
}


LL* createListNode(int value){
    LL* newNode = malloc(sizeof(LL));
    newNode->node = value;
    newNode->next = NULL;
    return newNode;
}

List* initList(){
    List* newList = malloc(sizeof(List));
    newList->head = NULL;
    return newList;
}

void insertListNode(int value, List* L){
    LL* myNode = createListNode(value);
    myNode->next = L->head;
    L->head = myNode;
}

int ListSearch(int target, LL* head, int attempts){
    while(head != NULL && head->node != target){
        head = head->next;
        attempts++;
    }
    return attempts;
}

void freeList(List* L){
    LL* currNode = L->head;
    while(currNode != NULL){
        LL* next = currNode->next;
        free(currNode);
        currNode = next;
    }
    free(L);
}


Tn* createTreeNode(int value){
    Tn* newTn = malloc(sizeof(Tn));
    newTn->value = value;
    newTn->left = NULL;
    newTn->right = NULL;
    return newTn;
}

Tree* initTree(){
    Tree* newTree = malloc(sizeof(Tree));
    newTree->root = NULL;
    return newTree;
}

Tn* insertTreeNode(int value, Tn* root){
    if(root == NULL){
        return createTreeNode(value);
    }
    if(value < root->value){
        root->left = insertTreeNode(value, root->left);
    }else if(value > root->value){
        root->right = insertTreeNode(value, root->right);
    }
    return root;
}

int BSTSearch(int target, Tn* currNode, int attempts){
    if(currNode == NULL || currNode->value == target){
        return attempts;
    }
    if(target < currNode->value){
        return BSTSearch(target, currNode->left, attempts+1);
    }
    return BSTSearch(target, currNode->right, attempts+1);
}

void freeTreeNodes(Tn* root){
    if(root == NULL){
        return;
    }
    freeTreeNodes(root->left);
    freeTreeNodes(root->right);
    free(root);
}

void freeTree(Tree* T){
    freeTreeNodes(T->root);
    free(T);
}

int main(){
    FILE* arquivo = fopen("resultados.csv", "w");
    if(arquivo == NULL){
        printf("Erro ao abrir resultados.csv\n");
        return 1;
    }
    fprintf(arquivo, "n,estrutura,media\n");

    int tamanhos[] = {1000, 2000, 4000, 8000, 16000, 32000};
    int numTamanhos = sizeof(tamanhos) / sizeof(tamanhos[0]);
    int numBuscas = 5000;

    srand(time(NULL));

    for(int t = 0; t < numTamanhos; t++){
        int numValores = tamanhos[t];

        int* chaves = malloc(numValores * sizeof(int));
        for(int i = 0; i < numValores; i++){
            chaves[i] = i;
        }
        for(int i = numValores - 1; i > 0; i--){
            int j = randomNumber(i + 1);
            int temp = chaves[i];
            chaves[i] = chaves[j];
            chaves[j] = temp;
        }

        List* myList = initList();
        Tree* myTree = initTree();
        for(int i = 0; i < numValores; i++){
            insertListNode(chaves[i], myList);
            myTree->root = insertTreeNode(chaves[i], myTree->root);
        }

        long long somaLista = 0;
        long long somaBST = 0;
        for(int i = 0; i < numBuscas; i++){
            int target = chaves[randomNumber(numValores)];
            somaLista += ListSearch(target, myList->head, 1);
            somaBST += BSTSearch(target, myTree->root, 1);
        }

        fprintf(arquivo, "%d,Lista,%.2f\n", numValores, (double)somaLista / numBuscas);
        fprintf(arquivo, "%d,ABB,%.2f\n", numValores, (double)somaBST / numBuscas);
        printf("n = %d concluído\n", numValores);

        freeList(myList);
        freeTree(myTree);
        free(chaves);
    }

    fclose(arquivo);

    return 0;
}