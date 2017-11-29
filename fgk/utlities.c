#include <stdio.h>
#include <math.h>
#include "utilities.h"

#ifndef ALGORITMI_FGK_COMPRESSION_HUFFMANTREE_H
    #include "huffmantree.h"
#endif

// Utilities Functions

/* Prints some information about the Huffman Tree
 * like HT address, Tree address, Root Node Number, Root Element, isNYT(root)
 */
void printHuffmanTreeInfo(HuffmanTree *ht){
    printf("HT:\t %p\n", ht);
    printf("Tree:\t %p\n", ht->tree);
    printf("Root:\t %p (NN: %d, Element: %d, isNYT: %d), \n", ht->root, ht->root->node_number, ht->root->element, isNYT(ht->root));
    printHuffmanTree(ht);
}

void printHuffmanTree(HuffmanTree *ht){
    printTree(ht->root, 0);
    printf("----\n");
    printHuffmanArray(ht);
}

void printHuffmanArray(HuffmanTree* ht){
    printf("HTA:\n");
    printf("------\n");
    int i;
    for(i=0; i< HUFFMAN_ARRAY_SIZE; i++){
        if(ht->tree[i] == NULL){
            printf(" ");
        } else {
            printElement(ht->tree[i]);
            //printf("%x (%d) @%d", ht->tree[i]->element&0xff, ht->tree[i]->weight, ht->tree[i]->node_number);
        }
        if(i<HUFFMAN_ARRAY_SIZE-1){
            printf(",");
        }
    }
    printf("------\n");
}

void printTree(Node* root, int level){

    /* Expected output
     *
     * graph G {
        32 -- "f (11)";
        32 -- 21 -- 10 -- "c (5)";
        10 -- 5 -- "a (2)";
        5 -- "b (3)";
        21 -- 11 -- "d (5)";
        11 -- "e (6)";
       }
     *
     */

    // View online w/ any DOT visualizer (graphviz?).
    // We use webgraphviz.com

    if(root == NULL){
        return;
    }

    if(level == 0){
        printf("graph G {\n");
    }

    printElement(root);

    if(root->left == NULL && root->right == NULL){
        printf(";\n");
    }
    else{
        printf(" -- ");
        printTree(root->left, level+1);
        printElement(root);
        if(root->right != NULL) {
            printf(" -- ");
            printTree(root->right, level + 1);
        }

    }

    if(level == 0){
        printf("}\n");
    }
}

void printElement(Node* root){
    // Node:
    // character (weight, node number)

    if(isNYT(root)){
        printf("\"NYT (%d,%d)\"", root->weight, root->node_number);
    }
    else if(root->element == -1){
        printf("\"(%d,%d)\"", root->weight, root->node_number);
    } else {
        printf("\"%x (%d, %d)\"",root->element&0xff, root->weight, root->node_number);
    }
}

/*
 * Return the node level
 *          A
 *      B       C
 *   D    E   F   G
 *
 *   A is at NL 0
 *   B&C are at NL 1
 *   D,E,F,G are at NL 2
 */

int getNodeLevel(Node* node){
    if(node->parent == NULL){
        return 0;
    }

    Node* el = node->parent;
    int level = 0;
    while(el != NULL){
        level++;
        el = el->parent;
    }
    return level;
}

// Get node position in ht->elements array
int getNodePosition(HuffmanTree* ht, Node* node){
    int nl = getNodeLevel(node);
    //printf("Node is between %d and %d\n", (int) pow(2.0, nl)-1, (int) pow(2,nl+1)-2); // 0-indexed

    if(node == NULL){
        return-1;
    }

    int nn = (int) pow(2,nl);
    while(node->parent != NULL){
        nl--;
        if(node->parent->left == node){
            // 0
        } else {
            //printf("Adding 2^%d\n", nl-1);
            nn += pow(2, nl);
        }
        node = node->parent;
    }
    return nn-1;
}