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


Node* highest_numbered_node(HuffmanTree* ht, int weight){
    int i,k;
    int max_nn = 0;
    int stop = 0;
    Node* result = NULL;
    for(i=0; i<HA_DIM_X; i++){
        for(k=0; k<HA_DIM_Y; k++){
            Node* curr = ht->tree[i* HA_DIM_X + k];
            if(curr == NULL){
                continue;
            }

            if(curr->weight == weight && curr->node_number > max_nn){
                max_nn = curr->node_number;
                result = curr;
                stop = 1;
            }
        }
        if(stop == 1){
            // We're not interested in nodes that are below the ones with required weight
            // because they have a lower NN.
            break;
        }
    }
    return result;
}

void node_positioner(HuffmanTree* ht, Node* target){
    Node* last = highest_numbered_node(ht, target->weight);
    char buffer[250];
    char* element1 = getElement(last);
    sprintf(buffer, "Highest numbered node (aka LAST): %s", element1);
    debug(buffer);
    if(element1 != NULL){
        free(element1);
    }

    if(last != target) {
        debug("[node_positioner] LAST != TARGET, swapping!");
        char* element2 = getElement(target);
        sprintf(buffer, "[node_positioner] TARGET: %s", element2);
        debug(buffer);
        free(element2);
        swap_nodes(ht, target, last);
    }
    char* element = getElement(target);
    sprintf(buffer, "Updating weight for %s", element);
    free(element);
    debug(buffer);
    target->weight++;
}



char* get_po2(HuffmanTree *ht, int *bytes, int *bits){
    char* po2 = calloc(1, 9);

    int i;
    int is_byte = 0;
    *bytes = 0;
    *bits = 0;
    int partial_length = ht->partial_output_length;

    printf("Partial output: %s\n", ht->partial_output);
    printf("Partial length: %d\n", partial_length);
    int o_braces = 0;

    int partial_strlength = (int) strlen(ht->partial_output);
    if(partial_strlength != 0) {
        for (i = 0; i < partial_length; i++) {
            char current_c = ht->partial_output[i];
            printf("Current c: %c\n", current_c);
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
                sprintf(po2, "%s%s", po2, result);
                free(result);
                (*bytes)++;
            } else {
                if (current_c == '1' || current_c == '0') {
                    sprintf(po2, "%s%c", po2, current_c);
                    (*bits)++;
                } else {
                    ht->partial_output_length--;
                }
            }
        }
        printf("Partial Output 2: %s (%d)\n", po2, (int) strlen(po2));
    }

    //printf("(In get_po2) Bytes: %d, Bits: %d\n", *bytes, *bits);

    return po2;
}

void huffman_partial_final_conversion(HuffmanTree* ht){
    // Add some padding to fill the last byte.
    int* bytes = malloc(sizeof(int));
    int* bits = malloc(sizeof(int));
    char* result = get_po2(ht, bytes, bits);
    int* length = malloc(sizeof(int));
    int i;

    printf("[huffman_partial_final_conversion] Output was: %s\n", result);
    printf("[huffman_partial_final_conversion] Padded output is:\n");

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
    printf("\n");
}

void huffman_partial_convert_clear(HuffmanTree* ht){
    // Converts some part of the partial_output to output, then clear parts of its content
    // https://photos.app.goo.gl/htEQWOnttDyag90U2

    int* bits = malloc(sizeof(int));
    int* bytes = malloc(sizeof(int));
    int i;

    char* po2 = get_po2(ht, bytes, bits);

    printf("Bytes: %d, bits: %d\n", *bytes, *bits);

    free(ht->partial_output);
    ht->partial_output = calloc(HUFFMAN_ARRAY_SIZE, sizeof(char));
    ht->partial_output_length -= 3* *bytes;
    ht->partial_output_length -= *bits;

    size_t po2_length = (size_t) strlen(po2);
    if(po2_length % 8 != 0){
        printf("%d bits will be kept in partial_output\n", (int) po2_length % 8);
        for(i=(int) (po2_length - po2_length % 8); i < po2_length; i++){
            printf("i: %d\n", i);
            sprintf(ht->partial_output, "%s%c", ht->partial_output, po2[i]);
        }
    }


    char* final_output = calloc(HUFFMAN_ARRAY_SIZE, sizeof(char));
    strncpy(final_output, po2, (size_t) po2_length - po2_length % 8);
    free(po2);
    int length = 0;
    printf("Final Output: %s (%d)\n", final_output, (int) strlen(final_output));

    char* output = bin2byte(final_output, &length);
    free(final_output);
    printf("Output is: \n");

    free(ht->output);
    ht->output = calloc(20, sizeof(char));

    int j = 0;
    for(i = 0; i<length; i++){
        ht->output[length - 1 - i] = output[i];
        printf("0x%02x ", output[length - 1 - i] & 0xff);
    }
    ht->output_length = length;
    printf("\n");

    free(output);
    free(bits);
    free(bytes);

    printf("reminder: %s\n", ht->partial_output);

    ht->partial_output_length += po2_length%8;

    printf("\n");
}

void endHuffman(HuffmanTree* ht){
    printf("We still have %d chars!\n", ht->partial_output_length);
    printf("Partial (and final) output: %s\n", ht->partial_output);
    huffman_partial_final_conversion(ht);
}

void huffman_append_partial_path(HuffmanTree* ht, char* path){
    if(ht != NULL) {
        debug("[huffman_append_partial_path]");
        huffman_partial_convert_clear(ht);
        ht->partial_output_length+= strlen(path);
        sprintf(ht->partial_output, "%s%s", ht->partial_output, path);
    }
}
void huffman_append_partial_new_element(HuffmanTree* ht, char* nyt_path, char element){
    if(ht != NULL) {
        huffman_partial_convert_clear(ht);
        ht->partial_output_length+= strlen(nyt_path) + 3; // path length + ( + char + )
        sprintf(ht->partial_output, "%s%s(%c)", ht->partial_output, nyt_path, element);
    }
}

HuffmanTree* add_new_element(HuffmanTree* ht, char c){
    Node* node = ht->root;
    Node* target = find_node(node, c);

    // Is the character already in the tree?
    free(ht->output);
    ht->output = calloc(1, 50);

    int* length = malloc(sizeof(int));
    *length = 0;

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
        bytes[*length] = '\0';

        sprintf(ht->output, "%s", bytes);
        ht->output_length = *length;

        huffman_append_partial_path(ht, bytes);

        free(path);
        free(encoded_byte);
        free(length);

    } else {
        char* path = node_path(ht->nyt);
        char* encoded_byte = bin2byte(path, length);

        int i;
        char bytes[*length + 1];
        for(i = 0; i<*length; i++){
            bytes[i] = encoded_byte[i];
        }

        bytes[*length] = '\0';
        sprintf(ht->output, "%s%c", bytes, c);
        ht->output_length = *length;

        huffman_append_partial_new_element(ht, path, c);

        free(path);
        free(encoded_byte);
        free(length);
        ht->elements++;
        debug("[add_new_element] NS");
        Node* old_nyt = ht->nyt;
        Node* new_nyt = createNYT(old_nyt->node_number - 2);
        Node* new_char = createNode(old_nyt->node_number - 1, 1, c, NULL, NULL, old_nyt);



        int* old_nyt_position = getNodePosition(ht, old_nyt);
        int new_nyt_position[2] = {old_nyt_position[0] + 1, old_nyt_position[1] * 2};
        int new_char_position[2] = {new_nyt_position[0], new_nyt_position[1] + 1};

        char* string = getElement(old_nyt);
        free(string);

        char buffer[500];
        sprintf(buffer,"[add_new_element] old_nyt_pos: [%d][%d], new_nyt_pos: [%d][%d]", old_nyt_position[0], old_nyt_position[1], new_nyt_position[0], new_nyt_position[1]);
        debug(buffer);
        free(old_nyt_position);

        old_nyt->weight++;
        old_nyt->left = new_nyt;
        old_nyt->right = new_char;
        old_nyt->element = -1;


        ht->nyt = new_nyt;

        new_nyt->parent = old_nyt;
        new_char->parent = old_nyt;

        ht->tree[new_nyt_position[0] * HA_DIM_X + new_nyt_position[1]] = new_nyt;
        ht->tree[new_char_position[0] * HA_DIM_X + new_char_position[1]] = new_char;

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


//    if(target != NULL) {
//        // Yep, go to the node (target = node)
//        char string[50];
//        sprintf(string, "Target = %p", target);
//        debug(string);
//        Node* last;
//        int t = 0;
//        last = last_of_weight(ht->root, target->weight, &t);
//        //target->weight++;
//        if(last != target && last != NULL) {
//            swap_nodes(ht, target, last);
//        }
//
//    } else {
//        debug("Character not found");
//        Node *old_nyt = ht->nyt;
//        Node *new_nyt = createNYT(old_nyt->node_number - 2);
//        Node *new_char = createNode(old_nyt->node_number - 1, 1, c, NULL, NULL, old_nyt);
//
//
//        // Fix HT Array
//        /*
//         *       (1)               OLD NYT              --- OLD NYT LEVEL
//         *  (N)      (C)    NEW NYT     NEW CHAR        --- NEW NYT LEVEL = NEW CHAR LEVEL = OLD NYT LEVEL + 1
//         */
//        int old_nyt_position = getNodePosition(ht, ht->nyt);
//        // int old_nyt_level = getNodeLevel(ht->nyt);
//
//        // int new_nyt_level = old_nyt_level+1;
//        int new_nyt_position = 2 * (old_nyt_position + 1) - 1;
//        int new_char_position = new_nyt_position + 1;
//
//
//        //printf("NNPOS: %d\n", new_nyt_position);
//
//        // OLD NYT becomes our new parent for the new NYT and the new element
//        old_nyt->left = new_nyt;
//        old_nyt->right = new_char;
//        old_nyt->element = -1;
//        old_nyt->weight = 1;
//        new_nyt->parent = old_nyt;
//
//        ht->tree[old_nyt_position] = old_nyt;
//        ht->tree[new_nyt_position] = new_nyt;
//        ht->tree[new_char_position] = new_char;
//
//
//        // Set the new NYT pointer
//        ht->nyt = new_nyt;
//
//        if (ht->nyt == ht->root) {
//            ht->root = old_nyt;
//        }
//
//        if (ht->root->left != ht->nyt) {
//            //ht->root->weight++;
//        }
//
//        int size;
//        Node **level_siblings = siblings(ht, getNodeLevel(old_nyt), &size);
//        if (size != 0) {
//            // Last = level_siblings[size-1]
//
//            Node *last = level_siblings[size - 1];
//
//            if (last != NULL) {
//                printf("Level Siblings Size: %d\n", size);
//                printf("Last sibling: %p\n", last);
//                printf("Last sibling Element: %x\n", last->element);
//                printf("Last Sibling: %d\n", last->node_number);
//
//                if (last != old_nyt) {
//                    printf("We need to swap %d w/ %d\n", old_nyt->node_number, last->node_number);
//                    swap_nodes(ht, old_nyt, last);
//                }
//            } else {
//                printf("Last is null.\n");
//            }
//        }
//
//        free(level_siblings);
//        target = old_nyt; // p = parent of the new symbol node
//
//        if (target == ht->root) {
//            return ht;
//        }
//        target = target->parent;
//        weights_increment(node, target);
//    }
//    /*
//    while(target != ht->root) {
//        Node *highest = highest_numbered_node(ht, target->weight);
//        if(highest != target){
//            swap_nodes(ht, target, highest);
//        }
//        target->weight++;
//        target = target->parent;
//    }
//    /*if(target->parent == ht->nyt->parent){
//        Node* highest = highest_numbered_node(ht, target->weight);
//        if(highest != NULL){
//            swap_nodes(ht, target, highest);
//        }
//        free(highest);
//        target->weight++;
//        target = target->parent;
//    }
//
//    while(target != ht->root){
//        Node* swap_target = highest_numbered_node(ht, target->weight);
//
//        if(swap_target != NULL){
//            swap_nodes(ht, target, swap_target);
//        }
//
//        target->weight++;
//        target = target->parent;
//    }
//    */
//    return ht;
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

    int i, k;
    for(i = 0; i<HA_DIM_X; i++){
        for(k = 0; k<HA_DIM_Y; k++) {
            ht->tree[i * HA_DIM_X + k] = NULL;
            //printf("%p", ht->tree[i]);
        }
    }

    ht->tree[0] = ht->root;
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

void fillHTArrayFromTree(HuffmanTree* ht, Node* levelRoot, int originalLevel, int level, int i){
    if(levelRoot == NULL){
        return;
    }

    if(level == 0){
        int index = (int) pow(2,originalLevel) - 1 + i;
        char* element = getElement(levelRoot);
        char buffer[100];
        snprintf(buffer, 100, "[fillHTArrayFromTree] index: %d, i: %d, level: %d, originalLevel: %d, node: %s",
                index,
                 i,
                level,
                originalLevel,
                element
        );
        debug(buffer);
        free(element);

        ht->tree[originalLevel * HA_DIM_X + i] = levelRoot;
    } else {
        fillHTArrayFromTree(ht, levelRoot->left, originalLevel, level-1, 0);
        fillHTArrayFromTree(ht, levelRoot->right, originalLevel, level-1, 1);
    }
}

void generateHTArrayFromTree(HuffmanTree* ht){
    int i;
    for(i = 0; i < (int) (log(HUFFMAN_ARRAY_SIZE)/log(2)); i++){
        fillHTArrayFromTree(ht, ht->root, i, i, 0);
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


    int* pos1 = getNodePosition(ht, node);
    int* pos2 = getNodePosition(ht, node2);
    //int lvl1 = getNodeLevel(node);
    //int lvl2 = getNodeLevel(node2);

    char buffer[500];
    sprintf(buffer, "[Swapping] Pos1: [%d][%d] (NN %d), Pos2: [%d][%d] (NN %d)", pos1[0], pos1[1], node->node_number, pos2[0], pos2[1], node2->node_number);
    debug(buffer);

    free(pos1);
    free(pos2);



    if(node2->parent == node || node->parent == node2){
        error("[swap_nodes] I can't swap a child with its parent");
        return;
    }
    //int distance = pos1-pos2;


    swap_on_diff_lvls(ht, node, node2);

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
            printf("[create_subtree_from_node] pos: [%d][%d]\n", pos[0], pos[1]);
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

void rebuilding_from_array(HuffmanTree *ht, int pos, Node** arr, int i, int lvl){
    debug("[rebuilding_from_array] Starting rebuild");

    printPartialArray(arr);
    printHuffmanArray(ht);

    ht->tree[pos] = arr[i];
    if((2 *pos) + 1 < HA_DIM_X) {
        rebuilding_from_array(ht, (2 * pos) + 1, arr, i + (int) pow(2, lvl), lvl + 1);
        rebuilding_from_array(ht,  (2 * pos) + 2, arr, i + (int) pow(2, lvl) + 1, lvl + 1);
    }


}
void swap_on_diff_lvls(HuffmanTree* ht, Node* node, Node* node2){
    debug("Swapping on different levels");
    Node** arr = malloc(sizeof(Node*) * HA_DIM_X * HA_DIM_Y);
    Node** arr2 = malloc(sizeof(Node*) * HA_DIM_X * HA_DIM_Y);

    int i, k;
    for(i=0; i<HA_DIM_X; i++){
        for(k=0; k<HA_DIM_Y; k++){
            arr[i * HA_DIM_X + k] = NULL;
            arr2[i * HA_DIM_X + k] = NULL;
        }
    }

    int* pos;
    int* pos2;
    pos = getNodePosition(ht, node);
    pos2 = getNodePosition(ht, node2);

    int nullpos[2] = {0,0};

    debug("[swap_on_diff_lvls] Creating arr");
    create_subtree_from_node(ht, node, arr, nullpos);
    debug("[swap_on_diff_lvls] End arr creation");
    printNodeArray(arr);

    debug("[swap_on_diff_lvls] Creating arr2");
    create_subtree_from_node(ht, node2, arr2, nullpos);
    debug("[swap_on_diff_lvls] End arr2 creation");
    printNodeArray(arr2);


    rebuilding_from_array(ht, pos2[0] * HA_DIM_X + pos2[1], arr, 0, 0);
    rebuilding_from_array(ht, pos[0] * HA_DIM_X + pos[1], arr2, 0, 0);

    free(pos);
    free(pos2);
    free(arr);
    free(arr2);
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

void* siblings(HuffmanTree* ht, int level, int* size){
    // Slice of ht->array from 2^(level) - 1 to 2^(level+1)-2
    // Size = 2^(level)
    int array_size = (int) pow(2,level);
    *size = array_size;

    Node **siblings = malloc(sizeof(Node) * HA_DIM_Y);
    int i;

    for(i=0; i<HA_DIM_Y; i++){
        if(ht->tree[level * HA_DIM_X + i] != NULL)
        siblings[i] = ht->tree[level * HA_DIM_X + i];
    }

    return siblings;
}

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