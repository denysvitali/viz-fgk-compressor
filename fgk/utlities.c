#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <string.h>

#include "../defines.h"

#ifndef  ALGORITMI_FGK_COMPRESSION_UTILITIES_H
    #include "utilities.h"
    #include "../console.h"

#endif

#ifndef ALGORITMI_FGK_COMPRESSION_HUFFMANTREE_H
    #include "huffmantree.h"
#endif

// Utilities Functions

/* Prints some information about the Huffman Tree
 * like HT address, Tree address, Root Node Number, Root Element, is_nyt(root)
 */
void printHuffmanTreeInfo(HuffmanTree *ht){
    if(!DEBUG){
        return;
    }
    printf("HT:\t %p\n", ht);
    printf("Tree:\t %p\n", ht->tree);
    printf("Root:\t %p (NN: %d, Element: %d, is_nyt: %d), \n", ht->root, ht->root->node_number, ht->root->element,
           is_nyt(ht->root));
    printHuffmanTree(ht);
}

void printHuffmanTree(HuffmanTree *ht){
    if(!DEBUG && !TEST) {
        return;
    }
    if(ht == NULL){
        return;
    }
    if(ht->root == NULL){
        return;
    }
    printTree(ht->root, 0);
    printf("----\n");
    //printHuffmanArray(ht);
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
    if(!DEBUG){
        return;
    }

    int i;
    for(i=0; i<HUFFMAN_ARRAY_SIZE; i++){
        printElement(array[i]);
        printf(",");
    }
    printf("\n");
}

void printHuffmanArray(HuffmanTree* ht){
    if(DEBUG) {
        printf("HTA:\n");
        printf("------\n");
        printNodeArray(ht->tree);
        printf("\n------\n");
    }
}

char* getTree(Node* root, int level) {
    char* string = calloc(sizeof(char), 5*4096*1024);

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
    char* string = calloc(sizeof(char), 100);

    if(root == NULL){
        sprintf(string, "(nil)");
        return string;
    }


    // Node:
    // character (weight, node number)

    if(is_nyt(root)){
        sprintf(string, "\"NYT (%d,%d)\"", root->weight, root->node_number);
    }
    else if(root->element == -1){
        sprintf(string, "\"(%d,%d)\"", root->weight, root->node_number);
    } else {
#if UTILITIES_PRINT_CHARS
        sprintf(string, "\"%c (%d, %d)\"",root->element, root->weight, root->node_number);
#else
        sprintf(string, "\"%x (%d, %d)\"",root->element&0xff, root->weight, root->node_number);
#endif
    }
    return string;
}

void printElement(Node* root){
    if(!DEBUG){
        return;
    }
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
    //printf("Node is between %d and %d\n", (int) pow(2.0, nl)-1, (int) pow(2,nl+1)-2); // 0-indexed

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

char* bin2byte(char* bin, int* length) {
    // https://photos.app.goo.gl/njS3AUvGThpDMWqL2
    if(bin == NULL){
        return NULL;
    }
    int size = (int) strlen(bin);
    char *result;
    int i, j;

    if (size % 8 != 0) {
        // String isn't a multiple of 8, add padding
        int padding_size = 8 - (size % 8);
        char *new_bin = malloc((size + padding_size + 1) * sizeof(char));
        for (i = 0; i < size + padding_size; i++) {
            new_bin[i] = (i < size ? bin[i] : '0');
        }
        new_bin[size + padding_size] = '\0';
        result = bin2byte(new_bin, length);
        free(new_bin);
        return result;
    }

    // Size IS a multiple of 8 (e.g bin = '11111000')
    *length = size / 8;
    result = calloc(*length, sizeof(char));
    //printf("Returning result for %s\nLength: %d\n", bin, *length);
    for(i = 0; i<*length; i++){
        int dec = 0;
        for(j = 0; j<8; j++){
            //printf("bin[%d]\n", i*8+j);
            dec += (bin[i*8+j] == '1' ? pow(2, 7-j) : 0);
        }
        //printf("dec: %d\n", dec);
        result[*length - i - 1] = (char) dec;
    }
    return result;
}

unsigned short* byte2bit(char b){
    unsigned short* result = calloc(8, sizeof(unsigned short));

    unsigned int dec = (unsigned int) b;
    int i;

    for(i=0; i<8; i++){
        result[7-i] = (unsigned short) ((dec % 2) == 1 ? 1 : 0);
        dec -= dec%2;
        dec /= 2;
    }

    return result;
}

char* get_filename(char* string){
    int i;
    long length = strlen(string);
    int last_slash = -1;

    for(i=0; i<length; i++){
        if(string[i] == '/'){
            last_slash = i;
        }
    }
    char* output = calloc((size_t) length+1, sizeof(char));

    if(last_slash == -1){
        strcpy(output, string);
        return output;
    }

    if(last_slash == length - 1){
        output[0] = '\0';
        return output;
    }

    for(i = 0; i<length-last_slash; i++){
        output[i] = string[last_slash + 1 + i];
    }
    return output;
}

// Creates a file if it doesn't exist.
int file_touch(char* path){
    FILE* fh = fopen(path, "w+");
    if(!ferror(fh)){
        fclose(fh);
        return 1;
    }

    return 0;
}

int file_write(char* path, char* content){
    FILE* fh = fopen(path, "wb");
    if(!ferror(fh)){
        fwrite(content, strlen(content), sizeof(char), fh);
        fclose(fh);
        return 1;
    }

    return 0;
}

char* file_read(char* path, int* error){
    FILE* fh = fopen(path, "rb");

    char* buffer = calloc(1024*1024, sizeof(char));
    size_t file_size = 0;

    *error = ferror(fh);

    if(!(*error)){
        while(!feof(fh)){
            size_t read_size = fread(buffer, sizeof(char), 1024, fh);
            file_size += read_size;
        }

        printf("File size is: %d\n", (int) file_size);
        fclose(fh);

        if(file_size != 0){
            char* output = malloc(file_size+1);
            memcpy(output, buffer, file_size);
            output[file_size] = '\0';
            free(buffer);
            return output;
        }

        return buffer;
    }

    return NULL;
}

int file_delete(char* path){
    return remove(path);
}

char* path_to_string(unsigned short* path, int length){
    int i;
    char* path_string = calloc((length+1),sizeof(char));
    for(i=0; i<length; i++){
        strncat(path_string, (path[i] == 0?"0":"1"), 1);
    }
    path_string[length] = '\0';
    return path_string;
}

/*
 * Compares arr w/ arr2, if they are different returns 0.
 */

int compare_short_int(unsigned short* arr, unsigned short* arr2, int length){
    int i;
    for(i=0; i<length; i++){
        if(arr[i] != arr2[i]){
            return 0;
        }
    }

    return 1;
}
