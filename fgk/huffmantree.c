#include <stdlib.h>
#include <stdio.h>
#include <math.h>
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
    for(i=0; i<HUFFMAN_ARRAY_SIZE; i++){
        for(k=0; k<HUFFMAN_SYMBOLS; k++){
            Node* curr = ht->tree[i][k];
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

void weights_increment(Node* root, Node* node){
    node->weight++;
    if(node!=root){
        weights_increment(root, node->parent);
    }
}

void node_positioner(HuffmanTree* ht, Node* target){
    Node* last = highest_numbered_node(ht, target->weight);

    if(last != target){
        swap_nodes(ht, target, last);
    }

    target->weight++;
}

HuffmanTree* add_new_element(HuffmanTree* ht, char c){
    Node* node = ht->root;
    Node* target = find_node(node, c);

    // Character already is already in the tree?
    if(target != NULL){
        node_positioner(ht, target);
    } else {
        Node* old_nyt = ht->nyt;
        Node* new_nyt = createNYT(old_nyt->node_number - 2);
        Node* new_char = createNode(old_nyt->node_number - 1, 1, c, NULL, NULL, old_nyt);



        int* old_nyt_position = getNodePosition(ht, old_nyt);
        int new_nyt_position[2] = {old_nyt_position[0] + 1, old_nyt_position[1] * 2};
        int new_char_position[2] = {new_nyt_position[0], new_nyt_position[1] + 1};

        printf("OLD NYT: %s\n", getElement(old_nyt));

        char buffer[500];
        sprintf(buffer,"[add_new_element] old_nyt_pos: [%d][%d], new_nyt_pos: [%d][%d]", old_nyt_position[0], old_nyt_position[1], new_nyt_position[0], new_nyt_position[1]);
        debug(buffer);

        old_nyt->weight++;
        old_nyt->left = new_nyt;
        old_nyt->right = new_char;
        old_nyt->element = -1;


        ht->nyt = new_nyt;

        new_nyt->parent = old_nyt;
        new_char->parent = old_nyt;

        ht->tree[new_nyt_position[0]][new_nyt_position[1]] = new_nyt;
        ht->tree[new_char_position[0]][new_char_position[1]] = new_char;

        printHuffmanArray(ht);

        target = old_nyt;
    }

    while(target != ht->root){
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
    Node* tmp_nyt = createNYT(HUFFMAN_TOTAL_NODES);
    ht->root = tmp_nyt;
    ht->nyt = ht->root;

    int i, k;
    for(i = 0; i<HUFFMAN_ARRAY_SIZE; i++){
        for(k = 0; k<HUFFMAN_SYMBOLS; k++) {
            ht->tree[i][k] = NULL;
            //printf("%p", ht->tree[i]);
        }
    }

    ht->tree[0][0] = ht->root;
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

        ht->tree[originalLevel][index - (int) pow(2, level)] = levelRoot;
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





Node* last_of_weight(Node* root, int wtc, int* last){
    if(root->left == NULL && root->right == NULL){
        // Leaf, our root is an element
        //debug("LOW: 1");
        if(root->weight == wtc && root->node_number > *last){
            *last = root->node_number;
            return root;
        }
        //printf("Root weight: %d\nWTC: %d\nRoot NN: %d, *last: %d\n", root->weight, wtc, root->node_number, *last);
        //debug("Returning NULL");
        return NULL;
    }

    Node* res = NULL;
    Node* res2 = NULL;
    if(root->left != NULL){
        //debug("LOW: 2");
        res = last_of_weight(root->left, wtc, last);
    }
    if(root->right != NULL){
        //debug("LOW: 3");
        res2 = last_of_weight(root->right, wtc, last);
    }
    if(res == NULL && res2 == NULL){
        return NULL;
    }


    if(res == NULL){
        return res2;
    } else if(res2 == NULL){
        return res;
    }
    if(res->node_number > res2->node_number)
        return res;
    return res2;
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


    if(node2->parent == node || node->parent == node2){
        error("[swap_nodes] I can't a child with its parent");
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

    debug("End swap");
    printHuffmanTree(ht);

}

void create_subtree_from_node(HuffmanTree *ht, Node *node, Node ***result, int* pos){
    if(node != NULL) {
        if(pos[0] >= HUFFMAN_ARRAY_SIZE){
            return;
        }
        printf("[create_subtree_from_node] pos: [%d][%d]\n", pos[0], pos[1]);
        result[pos[0]][pos[1]] = node;
        //ht->tree[node->node_number] = NULL;

        int left[2] = {pos[0]+1, pos[1]*2};
        Node* n_left = node->left;
        int right[2] = {pos[0]+1, pos[1]*2 + 1};
        Node* n_right = node->right;

        if(DEBUG_SWAP_SHOW_ARRAYS) {
            printf("Pos: %d\n", pos);
            printf("Node itself: %s\n", getElement(node));

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
    //printf("Pos: %d\n", pos);
    int node_level = getLevel(pos);
    int pos_r = pos - (int) pow(2, node_level);
    if(pos<HUFFMAN_TOTAL_NODES){
        ht->tree[node_level][pos] = arr[i];
        if((2 * pos) + 1 < HUFFMAN_TOTAL_NODES) {
            rebuilding_from_array(ht, (2 * pos) + 1, arr, i + (int) pow(2, lvl), lvl + 1);
            rebuilding_from_array(ht, (2 * pos) + 2, arr, i + (int) pow(2, lvl) + 1, lvl + 1);
        }
    } else {
        error("[OUT OF BOUNDS] while rebuilding!");
    }
}
void swap_on_diff_lvls(HuffmanTree* ht, Node* node, Node* node2){
    debug("Swapping on different levels");
    Node ***arr = calloc(HUFFMAN_ARRAY_SIZE*HUFFMAN_TOTAL_NODES, sizeof(Node*));
    Node ***arr2 = calloc(HUFFMAN_ARRAY_SIZE*HUFFMAN_TOTAL_NODES, sizeof(Node*));

    int* pos;
    int* pos2;
    pos = getNodePosition(ht, node);
    pos2 = getNodePosition(ht, node2);
    debug("[swap_on_diff_lvls] Creating arr");
    create_subtree_from_node(ht, node, arr, pos);
    debug("[swap_on_diff_lvls] Creating arr2");
    create_subtree_from_node(ht, node2, arr2, pos2);


    if(DEBUG_SWAP_SHOW_ARRAYS) {
        int i;
        int nulls = 0;
        for (i = 0; i < 256; i++) {
            if (nulls >= 5) {
                printf("... (truncated)\n");
                break;
            }
            if (arr[i] == NULL) {
                nulls++;
            }
            printf("%d: %s\n", i, getElement(arr[i]));
        }
        nulls = 0;

        printf("Array 2\n");

        for (i = 0; i < 256; i++) {
            if (nulls >= 5) {
                printf("... (truncated)\n");
                break;
            }
            if (arr2[i] == NULL) {
                nulls++;
            }
            printf("%d: %s\n", i, getElement(arr2[i]));
        }
    }

    rebuilding_from_array(ht, pos2, arr, 0, 0);
    rebuilding_from_array(ht, pos, arr2, 0, 0);

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
void swap_nodes_array(HuffmanTree* ht, int pos, int pos2){
    Node* tmp;
    int nn;
    int level = getLevel(pos);
    int level2 = getLevel(pos2);

    int pos_r = pos - (int) pow(2, level);
    int pos2_r = pos2 - (int) pow(2, level2);

    tmp = ht->tree[level][pos_r];

    char db[50];
    sprintf(db, "swapping pos %d and %d", pos, pos2);
    debug(db);
    if(tmp == NULL || tmp->parent == NULL || tmp->parent->left == NULL) {
        debug("check4null true");
        return;
    }
    if(tmp == tmp->parent->left){
        debug("parent left");
        tmp->parent->left = ht->tree[level2][pos2_r];
    }else{
        if(tmp->parent->right != NULL) {
            debug("parent right");
            tmp->parent->right = ht->tree[level2][pos2_r];
        }
    }
    if(ht->tree[pos2] == NULL || ht->tree[level][pos2_r]->parent == NULL || ht->tree[level2][pos2_r]->parent->left == NULL ) {
        debug("check4null true");
        return;
    }
    if(ht->tree[level2][pos2] == ht->tree[level2][pos2_r]->parent->left){
        debug("parent 2 left");
        ht->tree[level2][pos2]->parent->left = tmp;
    }else{
        if(ht->tree[level2][pos2]->parent->right != NULL) {
            debug("parent 2 right");
            ht->tree[level2][pos2]->parent->right = tmp;
        }
    }
    nn =  ht->tree[level2][pos2]->node_number;
    ht->tree[level][pos] = ht->tree[level2][pos2];
    ht->tree[level][pos]->node_number = tmp->node_number;
    ht->tree[level2][pos2] = tmp;
    ht->tree[level2][pos2]->parent = ht->tree[level][pos]->parent;
    ht->tree[level][pos]->parent = tmp->parent;
    tmp->node_number = nn;
}

void swapping_array_recursive(HuffmanTree* ht, int pos, int pos2){
    swap_nodes_array(ht, pos, pos2);
    //int lvl = getNodeLevel(ht->tree[pos]), relative_lvl = lvl-subroot_lvl;

    int child_left = 2*pos+1;
    int child_left2 = 2*pos2+1;
    if(child_left >= HUFFMAN_ARRAY_SIZE || child_left2 >= HUFFMAN_ARRAY_SIZE){
        return;
    }
    swapping_array_recursive(ht, child_left, child_left2);
    swapping_array_recursive(ht, child_left+1, child_left2+1);
}

void update_weights_2(Node* root, Node* original, Node* start){
    if(start == NULL){
        warn("Update Weights called w/ start = NULL");
        return;
    } else {
        debug("Non è null");
    }

    if(start->parent != NULL && root == start->parent->parent)
    {
        //root->weight++;
        return;
    }
    check_move(root, start);
    if(start != original){
        start->weight++;
    }
    update_weights_2(root, original, start->parent);
}

void update_weights(Node* root, Node* start){
    if(start == NULL){
        warn("Update Weights called w/ start = NULL");
        return;
    } else {
        debug("Non è null");
    }

    /*if(root == start){
        return;
    }*/

    if(start->parent != NULL && root == start->parent->parent)
    {
        root->weight++;
        return;
    }
    check_move(root, start);
    update_weights_2(root, start, start->parent);
}

Node* check_move(Node* root, Node* node){
    /*first = find_node(root, c);
    if(first == NULL){
        return NULL;
    }*/

    // Debug
    char buffer[200];
    sprintf(buffer, "check_move called w/ %p (%d), %p (%d)", root, root->node_number, node, node->node_number);
    debug(buffer);

    Node* last;
    int l = 0;
    last = last_of_weight(root, node->weight, &l);
    if(last != NULL)
        printf("Last: %p %d\n", last, last->node_number);
    printf("Node: %p %d\n", node, node->node_number);
    if(node != last && node != root && last != root) {
        printf("Swapping %d w/ %d\n", node->node_number, last->node_number);
        // swap_nodes(node, last);
        int nn = node->node_number;
        node->node_number = last->node_number;
        last->node_number = nn;
    }
    return last;
}

void check_move_weight(Node* root, Node* node){
    Node* last = check_move(root, node);
    if(last == NULL){
        return;
    }
    last->weight++;
    update_weights(root,last->parent);
}

void* siblings(HuffmanTree* ht, int level, int* size){
    // Slice of ht->array from 2^(level) - 1 to 2^(level+1)-2
    // Size = 2^(level)
    int array_size = (int) pow(2,level);
    *size = array_size;

    Node **siblings = malloc(sizeof(Node) * array_size);
    int i;

    for(i=0; i<*size; i++){
        siblings[i] = ht->tree[*size - 1 + i];
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