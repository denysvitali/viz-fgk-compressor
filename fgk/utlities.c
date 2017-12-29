#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <string.h>

#ifndef  ALGORITMI_FGK_COMPRESSION_UTILITIES_H
    #include "utilities.h"
#include "../console.h"
#endif

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
    if(ht == NULL){
        return;
    }
    if(ht->root == NULL){
        return;
    }
    printTree(ht->root, 0);
    printf("----\n");
    printHuffmanArray(ht);
}

void saveHuffmanTree(HuffmanTree *ht, char* filename){
    char* ht_string = getTree(ht->root, 0);
    FILE* fh;
    if((fh = fopen(filename, "w+")) != NULL){
        debug("[Save Huffman Tree] Saving file...");
        if(!ferror(fh)){
            fprintf(fh, ht_string);
            fclose(fh);
        } else{
            char buffer[50];
            snprintf(buffer, 50, "Error: %d",  ferror(fh));
            error(buffer);
        }
    } else {
        error("[Save Huffman Tree] Unable to open FH");
    }
    free(ht_string);
}

void printNodeArray(Node** array){
    int i, k;
    for(i=0; i< HA_DIM_X; i++){ // We use HUFFMAN_NODES here to truncate the output
        for(k = 0; k<HA_DIM_Y; k++){
            if(k >= (int) pow(2, i)){
                break;
            }
            if(array[i*HA_DIM_X + k] == NULL){
                printf(" ");
            } else {
                printElement(array[i * HA_DIM_X + k]);
                //printf("%x (%d) @%d", ht->tree[i]->element&0xff, ht->tree[i]->weight, ht->tree[i]->node_number);
            }
            if(i<HUFFMAN_TOTAL_NODES-1){
                printf(",");
            }
        }
    }
}

void printHuffmanArray(HuffmanTree* ht){
    printf("HTA:\n");
    printf("------\n");
    printNodeArray(ht->tree);
    printf("\n------\n");
}

char* getTree(Node* root, int level) {
    char* string = calloc(sizeof(char), 500);

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
        return "";
    }

    if(level == 0){
        sprintf(string + strlen(string), "graph G {\n");
    }

    char* element_string;
    char* tree_string;
    element_string = getElement(root);
    sprintf(string + strlen(string), "%s", element_string);
    free(element_string);

    if(root->left == NULL && root->right == NULL){
        sprintf(string + strlen(string), ";\n");
    } else if(root->left == root || root->right == root){
        error("[getTree] Circular reference!");
        return string;
    }
    else{
        sprintf(string + strlen(string), " -- ");
        element_string = getElement(root);
        tree_string = getTree(root->left, level+1);

        sprintf(string + strlen(string), "%s", tree_string);
        sprintf(string + strlen(string), "%s", element_string);

        free(element_string);
        free(tree_string);

        tree_string = getTree(root->right, level + 1);

        if(root->right != NULL) {
            sprintf(string + strlen(string)," -- ");
            sprintf(string + strlen(string), "%s", tree_string);
        }

        free(tree_string);

    }

    if(level == 0){
        sprintf(string + strlen(string), "}\n");
    }

    return string;
}

void printTree(Node* root, int level){
    char* tree_string = getTree(root, level);
    printf("%s", tree_string);
    free(tree_string);
}

char* getElement(Node* root){

    if(root == NULL){
        return "(nil)";
    }

    char* string = calloc(sizeof(char), 100);

    // Node:
    // character (weight, node number)

    if(isNYT(root)){
        sprintf(string, "\"NYT (%d,%d)\"", root->weight, root->node_number);
    }
    else if(root->element == -1){
        sprintf(string, "\"(%d,%d)\"", root->weight, root->node_number);
    } else {
        sprintf(string, "\"%x (%d, %d)\"",root->element&0xff, root->weight, root->node_number);
    }
    return string;
}

void printElement(Node* root){
    if(root != NULL){
        char* elementText = getElement(root);
        printf("%s", elementText);
        free(elementText);
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
int* getNodePosition(HuffmanTree* ht, Node* node){
    int nl = getNodeLevel(node);
    int originalLvl = nl;
    printf("Node is between %d and %d\n", (int) pow(2.0, nl)-1, (int) pow(2,nl+1)-2); // 0-indexed

    if(node == NULL){
        return NULL;
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

    int node_position = nn - (int) pow(2, originalLvl);

    int* result = calloc(sizeof(int), 2);
    result[0] = originalLvl;
    result[1] = node_position;

    return result;
}

int checkNodeRelationships(Node* node){
    if(node == NULL){
        return -1;
    }

    int result;

    if(node->left != NULL){
        if(node->left->parent != node){
            return node->left->node_number;
        }
        result = checkNodeRelationships(node->left);

        if(result != 0){
            return result;
        }
    }

    if(node->right != NULL){
        if(node->right->parent != node){
            return node->right->node_number;
        }
        result = checkNodeRelationships(node->right);
        if(result != 0){
            return result;
        }
    }

    return 0;
}

// Checks the Huffman Tree for broken relationships
// Returns the first NN that breaks the relationship

int checkHuffmanRelationships(HuffmanTree* ht){
    if(ht->root == NULL){
        return -1;
    }

    return checkNodeRelationships(ht->root);
}


int getLevelInternal(int pos, int i){
    if(pos == 0){
        return i;
    }

    return getLevelInternal((pos-1)/2, i+1);
}

int getLevel(int pos){
    return getLevelInternal(pos, 0);
}

void printPartialArray(Node** arr){
    warn("Printing partial array!");
    int i, k;
    for(i=0; i<HA_DIM_X; i++){
        for(k=0; k<HA_DIM_Y; k++){
            if(arr[i*HA_DIM_X + k] != NULL){
                char* string = getElement(arr[i * HA_DIM_X + k]);
                printf("%s,", string);
                free(string);
            }
        }
        //printf("\n");
    }
    printf("\n");
}