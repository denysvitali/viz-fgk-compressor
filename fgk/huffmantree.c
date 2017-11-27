#include <stdlib.h>
#include <stdio.h>
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

        // OLD NYT becomes our new parent for the new NYT and the new element
        old_nyt->left = new_nyt;
        old_nyt->right = new_char;
        old_nyt->element = -1;
        old_nyt->weight = 1;

        //ht->root = old_nyt;
        //ht->root->weight++;

        // Set the new NYT pointer
        ht->nyt = new_nyt;
        printElement(ht->root);
        printf("\n");

        // Fix Weights
        int i = 0;
        Node* n = last_of_weight(ht->root, old_nyt->weight, &i);
        printf("Node: %p\n", n);
        printf("Root: %p\n", ht->root);

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

Node* find_node(Node *root, char c){
    if(root == NULL){
        return NULL;
    }
    printf("Node: %p\n", root);
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
    Node* tmp_nyt = createNYT(511);
    ht->root = tmp_nyt;
    ht->nyt = ht->root;
    ht->tree[0] = ht->root;
    return ht;
}


Node* check_move(Node *root, char c){
    Node* first;
    Node* last;
    first = find_node(root, c);
    int l = 0;
    last = last_of_weight(root, first->weight, &l);
    if(first != last) {
        swap_nodes(first, last);
    }
    return last;
}

void check_move_weight(Node* root, char c){
    Node* last = check_move(root, c);
    last->weight++;
    update_weights(last->parent);
}



Node* last_of_weight(Node* root, int wtc, int* last){
    if(root->left == NULL && root->right == NULL){
        // Leaf, our root is an element
        debug("LOW: 1");
        if(root->weight == wtc && root->node_number < *last){
            *last = root->node_number;
            return root;
        }
        printf("Root weight: %d\nWTC: %d\nRoot NN: %d, *last: %d\n", root->weight, wtc, root->node_number, *last);
        debug("Returning NULL");
        return NULL;
    }

    Node* res;
    if(root->left != NULL){
        debug("LOW: 2");
        res = last_of_weight(root->left, wtc, last);
        if(res != NULL) {
            if(res->node_number < *last)
                return res;
        } else{
            warn("Res is null [2]");
        }
    }
    if(root->right != NULL){
        debug("LOW: 3");
        res = last_of_weight(root->right, wtc, last);
        if(res != NULL) {
            if (res->node_number < *last)
                return res;
        } else{
            warn("Res is null [3]");
        }
    }
    return NULL;
}

void swap_nodes(Node* node, Node* node2){
    Node* aux = node;
    *node = *node2;
    *node2 = *aux;
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
void update_weights(Node* start){
    if(start == NULL){
        return;
    }
    check_move(start, start->element);
    start->weight++;
    update_weights(start->parent);
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