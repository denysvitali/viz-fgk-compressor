#include <stdio.h>
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
        printf("\"%X (%d, %d)\"",root->element&0xff, root->weight, root->node_number);
    }
}
