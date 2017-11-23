#ifndef ALGORITMI_FGK_COMPRESSION_HUFFMANTREE_H
#define ALGORITMI_FGK_COMPRESSION_HUFFMANTREE_H

#endif //ALGORITMI_FGK_COMPRESSION_HUFFMANTREE_H

typedef struct Node {
    int node_number;
    int weight;
    int element;
    struct Node* left;
    struct Node* right;
    struct Node* parent;
} Node;


typedef struct{
    Node* root;
    Node* tree[511];
    Node* nyt;
} HuffmanTree;

HuffmanTree* add_new_element(HuffmanTree* ht, char c);
HuffmanTree* createHuffmanTree();
Node* find_node(Node *root, char c);
Node* createNYT(int i);
Node* createNode(int node_number, int weight, int element, Node* left, Node* right, Node* parent);
Node* findNYT(Node* root);
int isNYT(Node *pNode);
Node* check_move(Node *root, char c);
void check_move_and_weight(Node* root, char c);
Node* last_of_weight(Node* root, int wtc, int* last);
void swap_nodes(Node* node, Node* node2);
void update_weights(Node* start);