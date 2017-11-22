#include <stdlib.h>
#include <stdio.h>
#include "utilities.h"
#ifndef ALGORITMI_FGK_COMPRESSION_HUFFMANTREE_H
    #include "huffmantree.h"
#endif

// Utilities Functions

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

    if(root->element == -1){
        printf("\"(%d,%d)\"", root->weight, root->node_number);
    } else {
        printf("\"%c (%d, %d)\"",root->element, root->weight, root->node_number);
    }
}
