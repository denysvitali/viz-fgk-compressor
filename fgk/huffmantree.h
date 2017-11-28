#ifndef ALGORITMI_FGK_COMPRESSION_HUFFMANTREE_H
#define ALGORITMI_FGK_COMPRESSION_HUFFMANTREE_H

#endif //ALGORITMI_FGK_COMPRESSION_HUFFMANTREE_H

#define HUFFMAN_ARRAY_SIZE 511

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
    Node* tree[HUFFMAN_ARRAY_SIZE];
    Node* nyt;
} HuffmanTree;

HuffmanTree* add_new_element(HuffmanTree* ht, char c);
HuffmanTree* createHuffmanTree();
Node* find_node(Node *root, int c);
Node* createNYT(int i);
Node* createNode(int node_number, int weight, int element, Node* left, Node* right, Node* parent);
Node* findNYT(Node* root);
int isNYT(Node *pNode);
Node* check_move(Node* root, Node* node);
void check_move_weight(Node* root, Node* node);
Node* last_of_weight(Node* root, int wtc, int* last);
void swap_nodes(HuffmanTree* ht, Node* node, Node* node2);
void update_weights(Node* root, Node* start);
void* siblings(HuffmanTree* ht, int level, int* size);