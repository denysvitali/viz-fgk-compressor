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
        new_nyt->parent = old_nyt;

        // Set the new NYT pointer
        ht->nyt = new_nyt;

        if(ht->nyt == ht->root){
            ht->root = old_nyt;
        }
        printElement(ht->root);
        printf("\n");

        // Fix Weights
        int i = 0;
        printf("LOW params: %p, %d, %d\n", ht->root, old_nyt->weight, i);
        Node* n = last_of_weight(ht->root, old_nyt->weight, &i);
        if(n != NULL) {
            printf("Node: %p, NN: %d\n", n, n->node_number);
        }
        //printf("LOW NN: %d\n", i);
        printf("Root: %p\n", ht->root);
        printf("Parent: %p\n", ht->nyt->parent);

        //update_weights(ht->nyt->parent);
        if(new_char->parent->parent != NULL){
            debug("Calling Update weights");
            // Printing tree before update weights
            debug("Print before u_w");
            printHuffmanTree(ht);
            update_weights(ht->root, new_char->parent);
            debug("Printing after u_w");
            printHuffmanTree(ht);
        }

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

    Node* res;
    Node* res2;
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

void swap_nodes(Node* node, Node* node2){
    Node* aux = node;
    if(node->parent == NULL || node2->parent == NULL){
        // Not going to swap a root.
        return;
    }

    if(aux->parent == node2->parent){
        printf("Parent L: %p R: %p\n", aux->parent->left, aux->parent->right);

        Node* left = node->parent->left;
        Node* right = node->parent->right;

        node->parent->left = right;
        node->parent->right = left;
        printf("Parent L: %p R: %p\n", aux->parent->left, aux->parent->right);
    }


    /*debug("Swapping nodes");
    if(aux->parent->left == node){
        debug("Node is on the left");
        // Node is on the left
        if(node2->parent->left == node2){
            // Node 2 is on the left
            debug("L");
            *aux->parent->left = *node2->parent->left;
        } else {
            // Node 2 is on the right
            debug("R");
            *aux->parent->left = *node2->parent->right;
        }
    } else {
        // Node is on the right
        debug("Node is on the right");
        if(node2->parent->left == node2){
            // Node 2 is on the left
            debug("L");
            *aux->parent->right = *node2->parent->left;
        } else {
            // Node 2 is on the right
            debug("R");
            *aux->parent->right = *node2->parent->right;
        }
    }

    *node = *node2;
    *node2 = *aux;*/
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
        swap_nodes(node, last);
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