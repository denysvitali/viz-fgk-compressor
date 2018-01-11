#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "../defines.h"
#include "../console.h"
#include "huffmantree.h"

#ifndef ALGORITMI_FGK_COMPRESSION_UTILITIES_H
    #include "utilities.h"
#endif


Node* highest_numbered_node(HuffmanTree* ht, Node* node){
    int i;

    Node* highest = node;
    if(node == NULL){
        return NULL;
    }
    for(i=node->node_number+1; i<HUFFMAN_ARRAY_SIZE; i++){
        if(ht->tree[i] != NULL){
            if(ht->tree[i]->weight == node->weight){
                highest = ht->tree[i];
            }
        }
    }

    if(DEBUG) {
        printElement(highest);
    }


    return highest;
}

void node_positioner(HuffmanTree* ht, Node* target){
    if(target == NULL){
        return;
    }
    Node* last = highest_numbered_node(ht, target);
    char buffer[250];

    if(DEBUG) {
        char *element1 = getElement(last);
        sprintf(buffer, "Highest numbered node (aka LAST w/ weight %d): %s", last->weight,  element1);
        debug(buffer);

        if (element1 != NULL) {
            free(element1);
        }
    }

    if(last != target && last != target->parent) {
        if(DEBUG) {
            debug("[node_positioner] LAST != TARGET, swapping!");
            char *element2 = getElement(target);
            sprintf(buffer, "[node_positioner] TARGET: %s", element2);
            debug(buffer);
            free(element2);
            char buffer[200];
            sprintf(buffer,
                    "[node_positioner] Found as highest %d and tried to swap it with %d",
                    last->node_number, target->node_number);
            debug(buffer);
        }
        swap_nodes(ht, target, last);
    }

    if(DEBUG) {
        char *element = getElement(target);
        sprintf(buffer, "Updating weight for %s", element);
        free(element);
        debug(buffer);
    }
    if(last != target->parent || target->parent == ht->root)
        target->weight++;
}


void huffman_coding_reset_partial_output(HuffmanTree* ht){
    int i;
    for(i=0; i<ht->partial_output_length; i++){
        ht->partial_output[i] = 0;
    }
    ht->partial_output_length = 0;
}

void huffman_coding_bitcheck(HuffmanTree* ht){
    if(ht->mask == 0x01){
        ht->mask = 0x80;

        ht->output[ht->output_length] = ht->partial_output[0];
        ht->partial_output[0] = 0;
#if DEBUG
        printf("Byte completed! 0x%02x, OL: %d\n", ht->output[ht->output_length] & 0xff, ht->output_length);
#endif
        ht->output_length++;

        ht->partial_output_length = 0;
        huffman_coding_reset_partial_output(ht);
    } else {
        ht->mask >>= 1;
#if DEBUG
        printf("Mask is now 0x%02x\n", ht->mask);
#endif
    }
}

void huffman_append_partial_path(HuffmanTree* ht, unsigned short* path, int path_size){
    if(ht != NULL) {
        debug("[huffman_append_partial_path]");
        int i;
        for(i=0; i<path_size; i++){
            if(path[i]) {
                ht->partial_output[ht->partial_output_length] |= ht->mask;
            }
            huffman_coding_bitcheck(ht);
        }

    }
}
void huffman_append_partial_new_element(HuffmanTree* ht, unsigned short* nyt_path, int path_size, char element){
    if(ht != NULL) {
        debug("[huffman_append_partial_new_element]");
        char* string = path_to_string(nyt_path, path_size);
        printf("[huffman_append_partial_new_element] NYT is at path: %s\n", string);
    }


    int i;
    for(i = 0; i<path_size; i++) {
        if(nyt_path[i]) {
            ht->partial_output[ht->partial_output_length] |= ht->mask;
        }
        huffman_coding_bitcheck(ht);
    }
    debug("[huffman_append_partial_new_element] Printing element bin");
    unsigned short* binary = byte2bit(element);
    for(i = 0; i<8; i++){
        printf("%d", binary[i]);
        if(binary[i]) {
            ht->partial_output[ht->partial_output_length] |= ht->mask;
        }
        huffman_coding_bitcheck(ht);
    }
    printf("\n");



}

void huffman_partial_final_conversion(HuffmanTree* ht){
    // Add some padding to fill the last byte.
    ht->mask = 0x01;
    ht->output_length++;
    huffman_coding_reset_partial_output(ht);
}

void endHuffman(HuffmanTree* ht){
    if(DEBUG) {
        printf("We still have %d chars!\n", ht->partial_output_length);
        printf("Partial (and final) output: %s\n", ht->partial_output);
    }
    huffman_partial_final_conversion(ht);
}

int is_compressor(HuffmanTree* ht){
    return ht->mode == H_MODE_COMPRESSOR;
}

HuffmanTree* add_new_element(HuffmanTree* ht, char c){
    Node* node = ht->root;
    Node* target = find_node(node, c);

    int* length = malloc(sizeof(int));
    *length = 0;

    char debug_buffer[100];
    sprintf(debug_buffer, "[add_new_element] Character is %02x", c & 0xff);
    debug(debug_buffer);

    int* path_length = malloc(sizeof(int));
    unsigned short* path;

    if(target != NULL){
        debug("[add_new_element] AS");
        path = node_path(target, path_length);
        node_positioner(ht, target);
        huffman_append_partial_path(ht, path, *path_length);
    } else {
        path = node_path(ht->nyt, path_length);

        if(is_compressor(ht)) {
            if (ht->elements == 0) {
                ht->output[0] = c;
                ht->output_length = 1;
            } else {
                huffman_append_partial_new_element(ht, path, *path_length, c);
            }
        }

        ht->elements++;
        debug("[add_new_element] NS");
        Node* old_nyt = ht->nyt;

        if(old_nyt == NULL){
            exit(51);
        }

        Node* new_nyt = createNYT(old_nyt->node_number - 2);
        Node* new_char = createNode(old_nyt->node_number - 1, 1, c, NULL, NULL, old_nyt);

        char* string = getElement(old_nyt);
        free(string);

        old_nyt->weight++;
        old_nyt->left = new_nyt;
        old_nyt->right = new_char;
        old_nyt->element = -1;


        ht->nyt = new_nyt;

        new_nyt->parent = old_nyt;
        new_char->parent = old_nyt;

        ht->tree[new_nyt->node_number] = new_nyt;
        ht->tree[new_char->node_number] = new_char;

        target = old_nyt;
    }

    free(path);
    free(path_length);

    while(target != ht->root){
        char dbg[200];
        char* element = getElement(target);
        sprintf(dbg, "[add_new_element] TARGET: %s", element);
        free(element);
        debug(dbg);
        if(target == NULL || target->parent == NULL){
            return NULL;
        }
        target = target->parent;
        node_positioner(ht, target);
    }
    return ht;
}

/* Used only in decompression mode! */
void decode_byte(HuffmanTree* ht, char byte){
    if(is_compressor(ht)){
        return;
    }
    printf("[decode_byte] Called w/ HT: %p, Byte: %02x\n", ht, byte & 0xff);
    printf("[decode_byte] HT->partial_output is \"%s\" (%d)\n", ht->partial_output, ht->partial_output_length);

    if(ht->elements == 0){
        debug("[decode_byte] Adding this element directly because it's the first one");
        ht->output[0] = byte;
        add_new_element(ht, byte);
        char* new_partial_output = calloc(HUFFMAN_ARRAY_SIZE, sizeof(char));
        free(ht->partial_output);
        ht->partial_output = new_partial_output;
        ht->partial_output_length = 0;
    } else {

        if((ht->decoder_flags & H_DECODER_FLAG_NEXT_IS_BYTE) > 0){
            debug("[decode_byte] Parsing the new character...");
            ht->decoder_flags ^= H_DECODER_FLAG_NEXT_IS_BYTE;
            int remaining_bits = 8 - ht->partial_output_length;
            char* newbits = byte2bit(byte);
            printf("[decode_byte] newbits: %s (%ld)\n", newbits, strlen(newbits));


            char* finalbyte = calloc(9, sizeof(char));
            strncat(finalbyte, ht->partial_output, (size_t) ht->partial_output_length);
            strncat(finalbyte, newbits, (size_t) remaining_bits);

            int next_byte_size = ((int) (remaining_bits/8) + 1) * 8;
            if(remaining_bits%8 == 0){
                next_byte_size = remaining_bits;
            }
            printf("[decode_byte] Next Byte Size %d\n", next_byte_size);

            char* new_partial_output = calloc(HUFFMAN_ARRAY_SIZE, sizeof(char));
            free(ht->partial_output);
            ht->partial_output = new_partial_output;
            strncat(ht->partial_output, newbits + remaining_bits, (size_t) next_byte_size-remaining_bits);
            int* length = malloc(sizeof(int));
            char* finalbyte_byte = bin2byte(finalbyte, length);
            printf("[decode_byte] finalbyte: 0x%02x\n", finalbyte_byte[0] & 0xff);
            add_new_element(ht, finalbyte_byte[0]);
            char* byte = malloc(sizeof(char)*2);
            sprintf(byte, "%c", finalbyte[0]);

            printf("[decode_byte][decoded] %c\n", finalbyte_byte[0]);

            strncat(ht->output, byte, 1);
            ht->output_length++;
        } else {
            strncat(ht->partial_output, byte2bit(byte), 8);
            ht->partial_output_length += 8;
        }

        while(ht->partial_output_length > 0 && (ht->decoder_flags & H_DECODER_FLAG_NEXT_IS_BYTE)==0) {

            Node *node = ht->root;
            int i = 0;

            char *bits = byte2bit(byte);
            char *temp = calloc(17, sizeof(char));
            strcat(temp, bits);

            for (i = 0; i < strlen(ht->partial_output); i++) {
                if (ht->partial_output[i] == '0') {
                    if (node->left == NULL) {
                        break;
                    }
                    node = node->left;
                } else if (ht->partial_output[i] == '1') {
                    if (node->right == NULL) {
                        break;
                    }
                    node = node->right;
                }
            }

            if (node->left != NULL || node->right != NULL) {
                // Not a leaf. Wait for more bytes.
                printElement(node);
                debug("[decode_byte] Leaf not reached. We'll try again w/ the next byte");
                return;
            }

            char *element = getElement(node);
            char d_buffer[200];
            sprintf(d_buffer, "Our element is %s", element);
            debug(d_buffer);

            printf("[decode_byte] Writing %d bytes (sizeof(ht->partial_output) = %ld) to new_partial_output from address %p (was %p)\n",
                   ht->partial_output_length - i,
                   (int) sizeof(ht->partial_output) / sizeof(char),
                   (void *) ht->partial_output + i,
                   (void *) ht->partial_output
            );

            char *new_partial_output = calloc(HUFFMAN_ARRAY_SIZE, sizeof(char));
            strncpy(new_partial_output, ht->partial_output + i, (size_t) ht->partial_output_length - i);
            free(ht->partial_output);
            ht->partial_output = new_partial_output;
            ht->partial_output_length -= i;

            printf("[decode_byte] ht->partial_output is now %s\n", ht->partial_output);


            if (is_nyt(node)) {
                debug("[decode_byte] Got a new character!");
                // Next 8 bits are the character.
                ht->decoder_flags |= H_DECODER_FLAG_NEXT_IS_BYTE;
            } else {
                // Output our character (node->element) to the new file (or ht->output, whatever.)
                debug("[decode_byte] This character is being repeated.");
                printf("[decode_byte] Character is %c\n", node->element);
                char *byte = malloc(sizeof(char) * 2);


                sprintf(byte, "%c", node->element);
                printElement(node);
                printf("[decode_byte][decoded] %c\n", node->element);
                strncat(ht->output, byte, 1);
                ht->output_length++;
            }

            printf("[decode_byte] Partial output is now: %s\n", ht->partial_output);
        }
    }
}

Node* findNYT(Node* root){
    if(is_nyt(root)){
        return root;
    }
    Node *left = findNYT(root->left);
    if(is_nyt(left)){
        return left;
    }

    Node *right = findNYT(root->right);
    if(is_nyt(right)){
        return right;
    }

    return NULL; // NYT not found (It should *NEVER* happen)
}

int is_nyt(Node *pNode) {
    if(pNode == NULL){
        return 0;
    }
    if(pNode->weight == 0 && pNode->element == NYT_ELEMENT){
        return 1;
    }
    return 0;
}

int is_internal_node(Node *pNode){
    if(pNode == NULL){
        return 0;
    }
    if(pNode->weight != 0 && pNode->element == -1){
        return 1;
    }
    return 0;
}

Node* find_node(Node *root, int c){
    if(root == NULL){
        return NULL;
    }
    //printf("Node: %p\n", root);
    if(root->left == NULL && root->right == NULL){
        // Leaf, our root is an element
        if(root->element == c){
            return root;
        }
        return NULL;
    }

    if(root->left == root || root->right == root){
        error("[find_node] Circular reference!");
        return NULL;
    }

    Node* res;
    if(root->left != NULL){
        res = find_node(root->left, c);
        if(res != NULL)
            return res;
    }
    else {
        return NULL;
    }
    if(root->right != NULL){
        res = find_node(root->right, c);
        if(res != NULL)
            return res;
    }
    return NULL;
}

Node* createNYT(int i) {
    Node* root = malloc(sizeof(Node));
    root->weight = 0;
    root->element = NYT_ELEMENT;
    root->node_number = i;
    root->left = NULL;
    root->right = NULL;
    root->parent = NULL;
    return root;
}


Node* createNode(int node_number, int weight, int element, Node* left, Node* right, Node* parent){
    Node* n = malloc(sizeof(Node));
    n->node_number = node_number;
    n->weight = weight;
    n->element = element;
    n->left = left;
    n->right = right;
    n->parent = parent;
    return n;
}

HuffmanTree* createHuffmanTree(){
    HuffmanTree* ht = malloc(sizeof(HuffmanTree));
    Node* tmp_nyt = createNYT(HUFFMAN_ARRAY_SIZE - 1);
    ht->root = tmp_nyt;
    ht->nyt = ht->root;
    ht->output = calloc(1, HUFFMAN_ARRAY_SIZE);
    ht->partial_output = calloc(1, HUFFMAN_ARRAY_SIZE);
    ht->output_length = 0;
    ht->partial_output_length = 0;
    ht->elements = 0;
    ht->decoder_flags = 0;
    ht->mode = H_MODE_COMPRESSOR;
    ht->mask = 0x80; // 1000 0000 (MSB)

    int i;
    for(i = 0; i<HUFFMAN_ARRAY_SIZE; i++){
        ht->tree[i] = NULL;
        ht->partial_output[i] = 0;
    }

    ht->tree[HUFFMAN_ARRAY_SIZE - 1] = ht->root;
    return ht;
}

void freeNode(Node* node){
    if(node->left == NULL && node->right == NULL){
        free(node);
        return;
    }

    if(node->left != NULL){
        freeNode(node->left);
    }

    if(node->right != NULL){
        freeNode(node->right);
    }

    free(node);
}

void freeHuffman(HuffmanTree* ht){
    freeNode(ht->root);
    free(ht->output);
    free(ht->partial_output);
    free(ht);
}

unsigned short* recurse_node_path(Node* node, int* length, unsigned short* node_path){
    if(node == NULL || node->parent == NULL){
        return node_path;
    }

    if(node->parent->left == node){
        node_path[*length] = 0;
    } else {
        node_path[*length] = 1;
    }
    (*length)++;

    return recurse_node_path(node->parent, length, node_path);
}

unsigned short* node_path(Node* node, int* length){
    unsigned short* ret = malloc(HUFFMAN_ARRAY_SIZE * sizeof(unsigned short));
    *length = 0;
    unsigned short* node_path = recurse_node_path(node, length, ret);
    unsigned short* inverted_node_path = malloc(*length * sizeof(unsigned short));

    int i;
    for(i=0; i<*length; i++){
        inverted_node_path[i] = node_path[*length - 1 -i];
    }
    free(node_path);
    return inverted_node_path;
}

void swap_nodes(HuffmanTree* ht, Node* node, Node* node2){
    //Node* aux = node;
    if(node == NULL || node2 == NULL){
        // Null Pointer Exception
        return;
    }
    if(node->parent == NULL || node2->parent == NULL){
        // Not going to swap a root.
        return;
    }


    if(node == node2){
        // Not going to swap two identical nodes.
        return;
    }

    debug("[swap_nodes] Begin Swap");
    //printHuffmanTree(ht);


    //int lvl1 = getNodeLevel(node);
    //int lvl2 = getNodeLevel(node2);

    if(DEBUG) {
        int* pos1 = getNodePosition(ht, node);
        int* pos2 = getNodePosition(ht, node2);
        char buffer[500];
        sprintf(buffer, "[swap_nodes] Pos1: [%d][%d] (NN %d), Pos2: [%d][%d] (NN %d)", pos1[0], pos1[1],
                node->node_number, pos2[0], pos2[1], node2->node_number);
        free(pos1);
        free(pos2);
    }



    if(node2->parent == node || node->parent == node2){
        error("[swap_nodes] I can't swap a child with its parent");
        char* node_el1 = getElement(node);
        char* node_el2 = getElement(node2);

        char buffer[200];
        sprintf(buffer, "[swap_nodes] Tried to swap %s w/ %s", node_el1, node_el2);
        error(buffer);
        saveHuffmanTree(ht, "error-1.dot");
        sprintf(buffer, "VIZ encountered an unexpected error. Please try again or report the bug at %s", PROJECT_URL);
        error(buffer);
        exit(2);
    }
    //int distance = pos1-pos2;


    // Swap nodes w/ pointers (easy!)

    Node* parent1 = node->parent;
    Node* parent2 = node2->parent;

    Node* parent1_left = parent1->left;
    Node* parent2_left = parent2->left;


    if(parent1_left == node){
        // node is on the left of its parent
        parent1->left = node2;
    } else {
        // node is on the right of its parent
        parent1->right = node2;
    }

    if(parent2_left == node2){
        // node2 is on the left of its parent
        parent2->left = node;
    } else {
        // node2 is on the right of its parent
        parent2->right = node;
    }

    // Swap Array

    ht->tree[node2->node_number] = node;
    ht->tree[node->node_number] = node2;

    // Fix Node Numbers
    int nn = node->node_number;
    node->node_number = node2->node_number;
    node2->node_number = nn;

    node->parent = parent2;
    node2->parent = parent1;

    debug("[swap_nodes] End swap");
}