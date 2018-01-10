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
    Node* last = highest_numbered_node(ht, target);
    char buffer[250];

    if(DEBUG) {
        char *element1 = getElement(last);
        sprintf(buffer, "Highest numbered node (aka LAST): %s", element1);
        debug(buffer);

        if (element1 != NULL) {
            free(element1);
        }
    }

    if(last != target) {
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
    target->weight++;
}



char* get_po2(HuffmanTree *ht, int *bytes, int *bits){
    char* po2 = calloc(50, sizeof(char));

    int i;
    int is_byte = 0;
    *bytes = 0;
    *bits = 0;
    int partial_length = ht->partial_output_length;

    if(DEBUG) {
        //printf("Partial output: %s\n", ht->partial_output);
        //printf("Partial length: %d\n", partial_length);
    }
    int o_braces = 0;
    int partial_strlength = (int) strlen(ht->partial_output);
    if(partial_strlength != 0) {
        for (i = 0; i < partial_length; i++) {
            char current_c = ht->partial_output[i];
            if(DEBUG){
                //printf("Current c: %c\n", current_c);
            }
            if (o_braces == 0 && current_c == '(') {
                o_braces++;
                is_byte = 1;
                continue;
            }

            if (o_braces == 1 && current_c == ')') {
                o_braces--;
                is_byte = 0;
                continue;
            }

            if (is_byte) {
                char* result = byte2bit(current_c);
                strcat(po2, result);
                free(result);
                (*bytes)++;
            } else {
                if (current_c == '1' || current_c == '0') {
                    char string[2];
                    sprintf(string, "%c", current_c);
                    strcat(po2, string);
                    (*bits)++;
                } else {
                    ht->partial_output_length--;
                }
            }
        }
        if(DEBUG){
            //printf("Partial Output 2: %s (%d)\n", po2, (int) strlen(po2));
        }
        return po2;
    }

    free(po2);
    return NULL;
    //printf("(In get_po2) Bytes: %d, Bits: %d\n", *bytes, *bits);
}

void huffman_partial_final_conversion(HuffmanTree* ht){
    // Add some padding to fill the last byte.
    int* bytes = malloc(sizeof(int));
    int* bits = malloc(sizeof(int));
    char* result = get_po2(ht, bytes, bits);
    int* length = malloc(sizeof(int));
    int i;

    if(DEBUG) {
        if(result != NULL) {
            printf("[huffman_partial_final_conversion] Output was: %s\n", result);
        }
        printf("[huffman_partial_final_conversion] Padded output is:\n");
    }

    free(ht->partial_output);
    ht->partial_output = calloc(HUFFMAN_ARRAY_SIZE, sizeof(char));
    char* byte_result = bin2byte(result,length);
    for(i = 0; i<*length; i++){
        printf("0x%02X ", byte_result[*length - 1 -i] & 0xff);
        ht->partial_output[i] = byte_result[*length - 1 - i];
    }
    free(byte_result);
    ht->partial_output_length = *length;
    free(bytes);
    free(bits);
    free(result);
    free(length);
    if(DEBUG){
        printf("\n");
    }
}

void huffman_partial_convert_clear(HuffmanTree* ht){
    // Converts some part of the partial_output to output, then clear parts of its content
    // https://photos.app.goo.gl/htEQWOnttDyag90U2

    int* bits = malloc(sizeof(int));
    int* bytes = malloc(sizeof(int));
    int i;

    char* po2 = get_po2(ht, bytes, bits);
    if(po2 == NULL){
        free(bits);
        free(bytes);
        return;
    }

    if(DEBUG) {
        //printf("Bytes: %d, bits: %d\n", *bytes, *bits);
    }

    free(ht->partial_output);
    ht->partial_output = calloc(HUFFMAN_ARRAY_SIZE, sizeof(char));
    ht->partial_output_length -= 3* *bytes;
    ht->partial_output_length -= *bits;


    size_t po2_length = (size_t) strlen(po2);
    if(po2_length % 8 != 0){
        if(DEBUG) {
            printf("[huffman_partial_convert_clear] %d bits will be kept in partial_output\n", (int) po2_length % 8);
        }
        for(i=(int) (po2_length - po2_length % 8); i < po2_length; i++){
            if(DEBUG){
                //printf("[huffman_partial_convert_clear] i: %d\n", i);
            }
            sprintf(ht->partial_output, "%s%c", ht->partial_output, po2[i]);
        }
    }


    char* final_output = calloc(HUFFMAN_ARRAY_SIZE, sizeof(char));
    strncpy(final_output, po2, (size_t) po2_length - po2_length % 8);
    free(bits);
    free(bytes);
    free(po2);
    int length = 0;

    if(DEBUG) {
        //printf("Final Output: %s (%d)\n", final_output, (int) strlen(final_output));
    }

    char* output = bin2byte(final_output, &length);
    free(final_output);

    if(DEBUG) {
        //printf("Output is: \n");
    }

    free(ht->output);
    ht->output = calloc(length, sizeof(char));

    char* d_bytes = calloc(10, sizeof(char));
    for(i = 0; i<length; i++){
        ht->output[length - 1 - i] = output[i];
        if(DEBUG) {
            //sprintf(d_bytes, "%s0x%02x ", d_bytes, ht->output[length - 1 - i] & 0xff);
        }
    }
    ht->output_length = length;

    //debug(d_bytes);

    if(DEBUG){
        //printf("\n");
    }

    free(d_bytes);

    free(output);

    if(DEBUG){
        //printf("reminder: %s\n", ht->partial_output);
    }

    ht->partial_output_length += po2_length%8;

    if(DEBUG){
        //printf("\n");
    }
}

void endHuffman(HuffmanTree* ht){
    if(DEBUG) {
        printf("We still have %d chars!\n", ht->partial_output_length);
        printf("Partial (and final) output: %s\n", ht->partial_output);
    }
    huffman_partial_final_conversion(ht);
}

void huffman_append_partial_path(HuffmanTree* ht, char* path){
    if(ht != NULL) {
        debug("[huffman_append_partial_path]");
        huffman_partial_convert_clear(ht);
        ht->partial_output_length+= strlen(path);
        strcat(ht->partial_output, path);
    }
}
void huffman_append_partial_new_element(HuffmanTree* ht, char* nyt_path, char element){
    if(ht != NULL) {
        huffman_partial_convert_clear(ht);
        ht->partial_output_length+= strlen(nyt_path) + 3; // path length + ( + char + )
        char newelement[50];
        sprintf(newelement, "%s(%c)", nyt_path, element);
        strcat(ht->partial_output, newelement);
    }
}

HuffmanTree* add_new_element(HuffmanTree* ht, char c){
    Node* node = ht->root;
    Node* target = find_node(node, c);

    //printHuffmanTree(ht);
    //printHuffmanArray(ht);

    // Is the character already in the tree?
    free(ht->output);
    ht->output = calloc(1, 50);

    int* length = malloc(sizeof(int));
    *length = 0;

    char debug_buffer[100];
    sprintf(debug_buffer, "[add_new_element] Character is %02x", c & 0xff);
    debug(debug_buffer);

    /*if(DEBUG){
        char filename[200];
        sprintf(filename, "d-%d.dot", ht->elements);
        saveHuffmanTree(ht, filename);
    }*/


    if(target != NULL){
        debug("[add_new_element] AS");
        char* path = node_path(target);
        node_positioner(ht, target);

        char* encoded_byte = bin2byte(path, length);
        int i;
        char bytes[*length + 1];
        for(i = 0; i<*length; i++){
            bytes[i] = encoded_byte[i];
        }

        sprintf(ht->output, "%s", bytes);
        ht->output_length = *length;

        if(length != 0){
            huffman_append_partial_path(ht, bytes);
        }


        //free(path);
        free(encoded_byte);
        free(length);
        free(path);

    } else {
        char* path = node_path(ht->nyt);
        char* encoded_byte = bin2byte(path, length);

        if(ht->elements == 0){
            // First element!
            ht->output[0] = c;
            ht->output_length = 1;
        } else {
            /*char bytes[*length + 1];
            for(i = 0; i<*length; i++){
                bytes[i] = encoded_byte[i];
            }

            bytes[*length] = '\0';*/
            /*sprintf(ht->output, "%s%c", bytes, c);
            ht->output_length = *length;*/

            if(DEBUG){
                printf("[add_new_element] Path: %s (= 0x%02x)\n", path, encoded_byte[0] & 0xff);
            }

            huffman_append_partial_new_element(ht, path, c);
        }

        free(path);
        free(encoded_byte);
        free(length);
        ht->elements++;
        debug("[add_new_element] NS");
        Node* old_nyt = ht->nyt;
        Node* new_nyt = createNYT(old_nyt->node_number - 2);
        Node* new_char = createNode(old_nyt->node_number - 1, 1, c, NULL, NULL, old_nyt);



        //int* old_nyt_position = getNodePosition(ht, old_nyt);
        //int new_nyt_position[2] = {old_nyt_position[0] + 1, old_nyt_position[1] * 2};
        //int new_char_position[2] = {new_nyt_position[0], new_nyt_position[1] + 1};

        char* string = getElement(old_nyt);
        free(string);

        //char buffer[500];
        //sprintf(buffer,"[add_new_element] old_nyt_pos: [%d][%d], new_nyt_pos: [%d][%d]", old_nyt_position[0], old_nyt_position[1], new_nyt_position[0], new_nyt_position[1]);
        //debug(buffer);
        //free(old_nyt_position);

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

    while(target != ht->root){
        char dbg[200];
        char* element = getElement(target);
        sprintf(dbg, "[add_new_element] TARGET: %s", element);
        free(element);
        debug(dbg);
        target = target->parent;
        node_positioner(ht, target);
    }
    return ht;
}

void decode_byte(HuffmanTree* ht, char byte){
    char debug_buffer[50];
    sprintf(debug_buffer, "[decode_byte] Decoding %02x", byte & 0xff);
    debug(debug_buffer);

    if(ht->elements == 0){
        ht->output[0] = byte;
        add_new_element(ht, byte);
    } else {

        if((ht->decoder_flags & H_DECODER_FLAG_NEXT_IS_BYTE) > 0){
            debug("[decode_byte] Parsing the new character...");
            ht->decoder_flags ^= H_DECODER_FLAG_NEXT_IS_BYTE;
            printf("[decode_byte] ht->partial_output: %s\n", ht->partial_output);
            int remaining_bits = 8 - ht->partial_output_length;
            char* newbits = byte2bit(byte);
            char finalbyte[9];
            strcat(finalbyte, ht->partial_output);
            strncat(finalbyte, newbits, remaining_bits);
            int* length = malloc(sizeof(int));
            char* finalbyte_byte = bin2byte(finalbyte, length);
            printf("[decode_byte] finalbyte: %s (%02x)\n", finalbyte, finalbyte_byte[0]);
        }

        char* result = byte2bit(byte);
        Node* node = ht->root;
        int i=0;
        for(i = 0; i<strlen(result); i++){
            if(DEBUG) {
                printf("[decode_byte] %c\n", result[i]);
                char* bits = byte2bit(byte);
                char* temp = calloc(17, sizeof(char));

                if(ht->partial_output_length > 0) {
                    debug("Size was > 0");
                    strcat(temp, ht->partial_output);
                }
                strcat(temp, bits);

                printf("[decode_byte] Temp is %s\n", temp);

                char* byte = calloc(8, sizeof(char));
                strncpy(byte, temp, 8);

                printf("[decode_byte] Temp: %s\n", temp);
                printf("[decode_byte] Byte: %s\n", byte);

                // Converted
                int* length = malloc(sizeof(int));
                char byte_result = bin2byte(byte, length);
                printf("[decode_byte] Byte result: 0x%02x (%d)\n", byte_result & 0xff, *length);
            }

            if(result[i] == '0'){
                if(node->left == NULL){
                    break;
                }
                node = node->left;
            } else if(result[i] == '1'){
                if(node->right == NULL){
                    break;
                }
                node = node->right;
            }
        }

        //i-=1;

        strncpy(ht->partial_output, result + i * sizeof(char), (size_t) 8 - i);
        ht->partial_output_length = 8-i;
        if(isNYT(node)){
            debug("Got a new character!");
            // Next 8 bits are the character.
            ht->decoder_flags |= H_DECODER_FLAG_NEXT_IS_BYTE;
        }

        printf("[decode_byte] Partial output is now: %s\n", ht->partial_output);
    }
    ht->elements++;
}

Node* findNYT(Node* root){
    if(isNYT(root)){
        return root;
    }
    Node *left = findNYT(root->left);
    if(isNYT(left)){
        return left;
    }

    Node *right = findNYT(root->right);
    if(isNYT(right)){
        return right;
    }

    return NULL; // NYT not found (It should *NEVER* happen)
}

int isNYT(Node *pNode) {
    if(pNode == NULL){
        return 0;
    }
    if(pNode->weight == 0 && pNode->element == NYT_ELEMENT){
        return 1;
    }
    return 0;
}

int isInternalNode(Node *pNode){
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
    Node* tmp_nyt = createNYT(HUFFMAN_TOTAL_NODES-2);
    ht->root = tmp_nyt;
    ht->nyt = ht->root;
    ht->output = calloc(1, HUFFMAN_ARRAY_SIZE);
    ht->partial_output = calloc(1, HUFFMAN_ARRAY_SIZE);
    ht->output_length = 0;
    ht->partial_output_length = 0;
    ht->elements = 0;
    ht->decoder_flags = 0;

    int i;
    for(i = 0; i<HUFFMAN_ARRAY_SIZE; i++){
        ht->tree[i] = NULL;
        ht->partial_output[i] = NULL;
    }

    ht->tree[511] = ht->root;
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

void fillHTArrayFromTree(HuffmanTree* ht, Node* levelRoot, int originalLevel, int level, int i, int index){
    /*if(levelRoot == NULL){
        return;
    }

    index++;
    if(level == 0){
        //int index = (int) pow(2,originalLevel) - 1 + i;
        char *element;
        if(DEBUG) {
            element = getElement(levelRoot);
            char buffer[100];
            snprintf(buffer, 100, "[fillHTArrayFromTree] index: %d, i: %d, pow2/2: %d, level: %d, originalLevel: %d, node: %s",
                     index,
                     i,
                     (int) pow(2, originalLevel)/2,
                     level,
                     originalLevel,
                     element
            );
            debug(buffer);
        }

        int firstElement = originalLevel * HA_DIM_X;

        int arrpos = firstElement + (int) pow(2, originalLevel)/2 * i + index - 1;

        ht->tree[arrpos] = levelRoot;
        if(DEBUG){
            char buffer[100];
            sprintf(buffer, "[fillHTArrayFromTree] tree[%d] (index: %d) is now set to %s\n", arrpos, index, element);
            debug(buffer);
            free(element);
        }
    } else {
        if(levelRoot->parent == NULL || levelRoot->parent->left == levelRoot) {
            fillHTArrayFromTree(ht, levelRoot->left, originalLevel, level - 1, 0, 0);
            fillHTArrayFromTree(ht, levelRoot->right, originalLevel, level - 1, 0, 1);
        } else {
            fillHTArrayFromTree(ht, levelRoot->left, originalLevel, level - 1, 1, 0);
            fillHTArrayFromTree(ht, levelRoot->right, originalLevel, level - 1, 1, 1);
        }
    }*/
}

void generateHTArrayFromTree(HuffmanTree* ht){
    int i;
    for(i = 0; i < (int) (log(HUFFMAN_ARRAY_SIZE)/log(2)); i++){
        fillHTArrayFromTree(ht, ht->root, i, i, 0, 0);
    }
}


char* node_path(Node* node){
    char* ret = calloc(1, HUFFMAN_ARRAY_SIZE);

    if(node == NULL || node->parent == NULL){
        return ret;
    }

    char* path = node_path(node->parent);

    if(node->parent->left == node){
        sprintf(ret, "%s0", path);
        free(path);
        return ret;
    }

    sprintf(ret, "%s1", path);
    free(path);
    return ret;
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

    debug("Begin Swap");
    //printHuffmanTree(ht);


    //int lvl1 = getNodeLevel(node);
    //int lvl2 = getNodeLevel(node2);

    if(DEBUG) {
        int* pos1 = getNodePosition(ht, node);
        int* pos2 = getNodePosition(ht, node2);
        char buffer[500];
        sprintf(buffer, "[Swapping] Pos1: [%d][%d] (NN %d), Pos2: [%d][%d] (NN %d)", pos1[0], pos1[1],
                node->node_number, pos2[0], pos2[1], node2->node_number);
        debug(buffer);
        free(pos1);
        free(pos2);
    }



    if(node2->parent == node || node->parent == node2){
        error("[swap_nodes] I can't swap a child with its parent");
        char* node_el1 = getElement(node);
        char* node_el2 = getElement(node2);

        char buffer[200];
        sprintf(buffer, "Tried to swap %s w/ %s", node_el1, node_el2);
        error(buffer);
        saveHuffmanTree(ht, "error-1.dot");
        exit(2);
        return; // TODO: Better error management.
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

    debug("End swap");
}

void create_subtree_from_node(HuffmanTree *ht, Node *node, Node** result, int* pos){
    if(pos[0] * HA_DIM_X + pos[1] >= HA_DIM_X * HA_DIM_Y){
        return;
    }
    if(node != NULL) {
        if(DEBUG) {
            //printf("[create_subtree_from_node] pos: [%d][%d]\n", pos[0], pos[1]);
        }
        result[pos[0] * HA_DIM_X + pos[1]] = node;
        //ht->tree[node->node_number] = NULL;

        int left[2] = {pos[0]+1, pos[1]*2};
        Node* n_left = node->left;
        int right[2] = {pos[0]+1, pos[1]*2 + 1};
        Node* n_right = node->right;

        if(DEBUG && DEBUG_SWAP_SHOW_ARRAYS) {
            printf("Pos: [%d][%d]\n", pos[0], pos[1]);
            char* string;
            string = getElement(node);
            printf("Node itself: %s\n", string);

            printf("LEFT: \n");
            printElement(n_left);
            printf("\nRIGHT: \n");
            printElement(n_right);
            printf("\n");
        }

        create_subtree_from_node(ht, n_left, result, left);
        create_subtree_from_node(ht, n_right, result, right);
    } else {
        //warn("[create_subtree_from_node] Node is null!");
    }
}

void rebuilding_from_array(HuffmanTree *ht, int pos, Node** arr, int iter, int lvl){
    //debug("[rebuilding_from_array] Starting rebuild");
    /*if(iter == 0 && DEBUG){
        printf("\n\n");
    }

    int rel_pos = pos % HA_DIM_X;

    char debug_b[200];
    sprintf(debug_b, "[rebuilding_from_array] Arguments: HT: %p, pos: %d, arr: %p, iter: %d, lvl: %d, rel_pos: %d",
            ht,
            pos,
            arr,
            iter,
            lvl,
            rel_pos
    );
    debug(debug_b);

    //printPartialArray(arr);
    //printHuffmanArray(ht);

    if(DEBUG){
        char* element1 = getElement(ht->tree[pos]);
        char* element2 = getElement(arr[iter]);
        printf("Setting ht->tree[%d] to arr[%d] (%s to %s)\n", pos, iter, element1, element2);
        free(element1);
        free(element2);
    }

    ht->tree[pos] = arr[iter];
    if(pos + HA_DIM_X + 0 + 1*pos%HA_DIM_X < HA_DIM_X * HA_DIM_Y && iter + HA_DIM_X + iter%HA_DIM_X + 1 < HA_DIM_X * HA_DIM_Y &&
            arr[iter] != NULL
            ) {
        rebuilding_from_array(ht, pos + HA_DIM_X + pos%HA_DIM_X,  arr, iter + HA_DIM_X + iter%HA_DIM_X + 0, lvl);
        rebuilding_from_array(ht, pos + HA_DIM_X + pos%HA_DIM_X, arr, iter + HA_DIM_X + iter%HA_DIM_X + 1, lvl);
    }*/
}



/*
int calculate_weight(Node* node){
    if(node->left == NULL && node->right == NULL)
        return node->weight;

    int res = 0, res2 = 0;
    if(node->left != NULL){
        res = calculate_weight(node->left);
    }
    if(node->right != NULL){
        res2 = calculate_weight(node->right);
    }
    return res+res2;
}
*/

/*
void update_numbers(HuffmanTree* ht){

}

void node_to_array(HuffmanTree* ht, Node* node){
    int i, f = 0, p;
    for(i = 0; i < 511; i++)
    {
        if(ht->tree[i] == node) {
            f = 1;
            p = i,
        }
    }
    if(f)
        ht->tree[p]->weight++;
    //else

}

int add_weight_to_element(Node* node, char c){

	if(node->left == NULL && node->right == NULL){
		// Leaf, our node is an element
	    	if(node->element == c){
			node->weight++;
			return 1;
		}
		return 0;
	}

	int res;
	if(node->left != NULL){
		res = add_weight_to_element(node->left, c);
		if(res == 1)
			return 1;
	}
	else {
		return 0;
	}
	if(node->right != NULL){
		res = add_weight_to_element(node->right, c);
		if(res == 1)
			return 1;
	}
	return 0;
}
 */