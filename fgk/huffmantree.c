#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "../defines.h"
#include "../console.h"
#include "huffmantree.h"

#ifndef ALGORITMI_FGK_COMPRESSION_UTILITIES_H
    #include "utilities.h"

#endif

HuffmanTree* add_new_element(HuffmanTree* ht, char c){
    Node* node = ht->root;
    Node* target = find_node(node, c);
    if(target != NULL) {
        // Add weight
        char string[50];
        sprintf(string, "Target = %p", target);
        debug(string);
    } else {
        debug("Character not found");
        Node* old_nyt = ht->nyt;
        Node* new_nyt = createNYT(old_nyt->node_number-2);
        Node* new_char = createNode(old_nyt->node_number-1, 1, c, NULL, NULL, old_nyt);


        // Fix HT Array
        /*
         *       (1)               OLD NYT              --- OLD NYT LEVEL
         *  (N)      (C)    NEW NYT     NEW CHAR        --- NEW NYT LEVEL = NEW CHAR LEVEL = OLD NYT LEVEL + 1
         */
        int old_nyt_position = getNodePosition(ht, ht->nyt);
        // int old_nyt_level = getNodeLevel(ht->nyt);

        // int new_nyt_level = old_nyt_level+1;
        int new_nyt_position = 2*(old_nyt_position+1)-1;
        int new_char_position = new_nyt_position + 1;


        //printf("NNPOS: %d\n", new_nyt_position);

        // OLD NYT becomes our new parent for the new NYT and the new element
        old_nyt->left = new_nyt;
        old_nyt->right = new_char;
        old_nyt->element = -1;
        old_nyt->weight = 1;
        new_nyt->parent = old_nyt;

        ht->tree[old_nyt_position] = old_nyt;
        ht->tree[new_nyt_position] = new_nyt;
        ht->tree[new_char_position] = new_char;


        // Set the new NYT pointer
        ht->nyt = new_nyt;

        if(ht->nyt == ht->root) {
            ht->root = old_nyt;
        }

        if(ht->root->left != ht->nyt){
            ht->root->weight++;
        }

        int size;
        Node** level_siblings = siblings(ht, getNodeLevel(old_nyt), &size);
        if(size != 0){
            // Last = level_siblings[size-1]

            Node* last = level_siblings[size-1];

            if(last != NULL) {
                printf("Level Siblings Size: %d\n", size);
                printf("Last sibling: %p\n", last);
                printf("Last sibling Element: %x\n", last->element);
                printf("Last Sibling: %d\n", last->node_number);

                if (last != old_nyt) {
                    printf("We need to swap %d w/ %d\n", old_nyt->node_number, last->node_number);
                    swap_nodes(ht, old_nyt, last);
                }
            } else {
                printf("Last is null.\n");
            }

        }

        free(level_siblings);


        //printHuffmanTree(ht);
        //printf("\n");


        //ht->root->weight++;
//        // Fix Weights
//        int i = 0;
//        printf("LOW params: %p, %d, %d\n", ht->root, old_nyt->weight, i);
//        Node* n = last_of_weight(ht->root, old_nyt->weight, &i);
//        if(n != NULL) {
//            printf("Node: %p, NN: %d\n", n, n->node_number);
//        }
//
//
//
//        //printf("LOW NN: %d\n", i);
//        printf("Root: %p\n", ht->root);
//        printf("Parent: %p\n", ht->nyt->parent);
//
//        //update_weights(ht->nyt->parent);
//        if(new_char->parent->parent != NULL){
//            debug("Calling Update weights");
//            // Printing tree before update weights
//            debug("Print before u_w");
//            printHuffmanTree(ht);
//            update_weights(ht->root, new_char->parent);
//            debug("Printing after u_w");
//            printHuffmanTree(ht);
//        }

        return ht;
    }
    return NULL;
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
    Node* tmp_nyt = createNYT(HUFFMAN_ARRAY_SIZE);
    ht->root = tmp_nyt;
    ht->nyt = ht->root;

    int i;
    for(i = 0; i<HUFFMAN_ARRAY_SIZE; i++){
        ht->tree[i] = NULL;
        //printf("%p", ht->tree[i]);
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
    free(ht);
}

HuffmanTree* generateHTFromArray(Node** array){
    HuffmanTree* ht = malloc(sizeof(HuffmanTree));
    ht->root = array[0];
    int i;
    for(i=0; i<HUFFMAN_ARRAY_SIZE;i++) {
        ht->tree[i] = array[i];
        if(isNYT(ht->tree[i])){
            ht->nyt = ht->tree[i];
        }
    }
    return ht;
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

    debug("Begin Swap");
    printHuffmanTree(ht);


    int pos1 = getNodePosition(ht, node);
    int pos2 = getNodePosition(ht, node2);
    int lvl1 = getNodeLevel(node);
    int lvl2 = getNodeLevel(node2);

    // TODO: Implement ht->tree switching here.
    // WIP

    printHuffmanArray(ht);

    char buffer[500];
    sprintf(buffer, "[Swapping] Pos1: %d (NN %d), Pos2: %d (NN %d)", pos1, node->node_number, pos2, node2->node_number);
    debug(buffer);

    //int distance = pos1-pos2;


    if(lvl1 == lvl2) {
        //int new_pos1 = pos1-distance;
        //int new_pos2 = pos2+distance;
        //swapping_array_recursive(ht, pos1, pos2);
        //int new_pos1 = pos2;
        //int new_pos2 = pos1;
        //swap_tree();

        /*
         *                         00                                   L: 0
         *         01                                02                 L: 1
         *    03         04                    05          06           L: 2
         * 07    08   09    10              11    12    13    14        L: 3
         *
         * =================== SWAP 01 w/ 02 ===================
         *
         *                         00                                   L: 0
         *         02                                01                 L: 1
         *    05         06                    03          04           L: 2
         * 11    12    13    14             07    08   09    10         L: 3
         *
         * 00,01,02,03,04,05,06,07,08,09,10,11,12,13,14
         * 00,02,01,05,06,03,04,11,12,13,14,07,08,09,10
         *   |1 |1 |--2--|--2--|-----4-----|-----4-----
         *   |
         *   |____[SWAP HERE]
         *
         * 01 => 02 (+1)
         * 03 => 05 (+2)
         * 04 => 06 (+2)
         * 06 => 10 (+4)
         * 07 => 11 (+4)
         * 08 => 12 (+4)
         * 09 => 13 (+4)
         *
         * 02 => 01 (-1)
         * 05 => 03 (-2)
         * 06 => 04 (-2)
         * 10 => 06 (-4)
         * 11 => 07 (-4)
         * 12 => 08 (-4)
         * 13 => 09 (-4)
         *
         */

        debug("Saving Huffman Tree File...");
        saveHuffmanTree(ht, "./ht_swap.dot");

        int i,j;
        int maxlevel = (int) (log(HUFFMAN_ARRAY_SIZE + 1) / log(2));
        maxlevel -= lvl1;

        int cpos1 = pos1;

        Node** ht_copy = malloc(sizeof(ht->tree));
        for(i = 0; i<sizeof(ht->tree)/sizeof(Node*); i++){
            ht_copy[i] = ht->tree[i];
        }

        for(i=0; i<maxlevel; i++){

            int elementIndex = cpos1;
            //printTree(generateHTFromArray(ht->tree)->root, 0);
            //printHuffmanArray(ht);
            printf("Currently at level %d / %d, ElementIndex: %d \n", i, maxlevel, elementIndex);

            //                                                      ROOT
            // 01 = 2^1 + 0 + 0    (LVL 0)             (1,509)                       41 (1,510)        [SWAP HAPPENS HERE]
            // 03 = 2^2 + 1 + 0    (LVL 1)    NYT (0,507)    42 (1,508)         NULL           NULL
            // 04 = 2^2 + 1 + 1    (LVL 1)
            // 07 = 2^3 + 3 + 0    (LVL 2)   NULL    NULL      NULL    NULL    NULL  NULL    NULL    NULL
            // 08 = 2^3 + 3 + 1    (LVL 2)
            // 09 = 2^3 + 3 + 2    (LVL 2)
            // 10 = 2^3 + 3 + 3    (LVL 2)  2^3 + 3 + (2^4 - 1)
            // a  = b
            // 1, 3 | 4, 7 | 8 | 9 | 10

            // 2^0, 2^2 - 1 | 2^2, 2^3 - 1 | 2^3 | 2^3 + 1 | 2^3 + 2

            int allNull = 1;

            for(j=(int) pow(2,i); j < ((int) pow(2,i+1)); j++) {
                int a =  j + (int) pow(2,i)-1;
                int b = (int) pow(2,i) + (int) pow(2,i)-1 + j;

                printf("a: %d, b = %d\n", a,b);

                Node* prev = ht->tree[a];
                ht_copy[a] = ht_copy[b];
                ht_copy[b] = prev;

                if(ht->tree[a] != NULL || ht->tree[b] != NULL){
                    allNull = 0;
                }
            }

            if(allNull){
                // If the HT is correct, there shouldn't be any node that has a NULL parent.
                // For this reason, we're going to stop the for loop here to save some resources / time.
                break;
            }
        }

        for(i=0; i<HUFFMAN_ARRAY_SIZE; i++){
            ht->tree[i] = ht_copy[i];
        }

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
    tmp = ht->tree[pos];
    char db[50];
    sprintf(db, "swapping pos %d and %d", pos, pos2);
    debug(db);
    if(tmp == NULL || tmp->parent == NULL || tmp->parent->left == NULL) {
        debug("check4null true");
        return;
    }
    if(tmp == tmp->parent->left){
        debug("parent left");
        tmp->parent->left = ht->tree[pos2];
    }else{
        if(tmp->parent->right != NULL) {
            debug("parent right");
            tmp->parent->right = ht->tree[pos2];
        }
    }
    if(ht->tree[pos2] == NULL || ht->tree[pos2]->parent == NULL || ht->tree[pos2]->parent->left == NULL ) {
        debug("check4null true");
        return;
    }
    if(ht->tree[pos2] == ht->tree[pos2]->parent->left){
        debug("parent 2 left");
        ht->tree[pos2]->parent->left = tmp;
    }else{
        if(ht->tree[pos2]->parent->right != NULL) {
            debug("parent 2 right");
            ht->tree[pos2]->parent->right = tmp;
        }
    }
    nn =  ht->tree[pos2]->node_number;
    ht->tree[pos] = ht->tree[pos2];
    ht->tree[pos]->node_number = tmp->node_number;
    ht->tree[pos2] = tmp;
    ht->tree[pos2]->parent = ht->tree[pos]->parent;
    ht->tree[pos]->parent = tmp->parent;
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