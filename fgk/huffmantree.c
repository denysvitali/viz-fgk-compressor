#include <stdlib.h>
#include <stdio.h>
#include "../defines.h"
#include "huffmantree.h"

HuffmanTree* add_new_element(HuffmanTree* ht, char c){
    Node* node = ht->root;
    Node* target = find_node(node, c);
    if(target != NULL) {
        // Add weight
    } else {
        Node* new_char = ht->nyt;
        Node* new_nyt = createNYT(new_char->node_number-2);
        Node* new_char_parent = createNode(new_char->node_number, 1, -1, new_nyt, new_char, ht->nyt->parent);
        ht->nyt = new_nyt;
        new_nyt->parent = new_char_parent;
        new_char->parent = new_char_parent;
        new_char->element = c;
        new_char->weight = 1;
        new_char->right = NULL;
        new_char->left = NULL;
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
    printf("Node: %p", root);
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


void check_and_move(Node* root, char c){
    Node* first;
    Node* last;
    first = find_node(root, c);
    int l = 0;
    last = last_of_weight(root, first->weight, &l);
    if(first != last) {
        swap_nodes(first, last);
    }
}

void check_move_and_weight(Node* root, char c){
    Node* first;
    Node* last;
    first = find_node(root, c);
    int l = 0;
    last = last_of_weight(root, first->weight, &l);
    if(first != last) {
        swap_nodes(first, last);
    }
    last->weight++;
    update_weights(last->parent);
}



Node* last_of_weight(Node* root, int wtc, int* last){
    if(root->left == NULL && root->right == NULL){
        // Leaf, our root is an element
        if(root->weight == wtc && root->node_number > *last){
            *last = root->node_number;
            return root;
        }
        return NULL;
    }

    Node* res;
    if(root->left != NULL){
        res = last_of_weight(root->left, wtc, last);
        if(res != NULL) {
            if(res->node_number > *last)
                return res;
        }
    }
    else {
        return NULL;
    }
    if(root->right != NULL){
        res = last_of_weight(root->left, wtc, last);
        if(res->node_number > *last)
            return res;
    }
    return NULL;
}

void swap_nodes(Node* node, Node* node2){
    Node* aux = node;
    node = node2;
    node2 = aux;
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
    check_and_move(start, start->element);
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