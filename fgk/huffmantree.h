#ifndef ALGORITMI_FGK_COMPRESSION_HUFFMANTREE_H
#define ALGORITMI_FGK_COMPRESSION_HUFFMANTREE_H

#endif //ALGORITMI_FGK_COMPRESSION_HUFFMANTREE_H

#define HUFFMAN_SYMBOLS 257
#define HUFFMAN_TOTAL_NODES ((2*HUFFMAN_SYMBOLS)-1)
#define HUFFMAN_ARRAY_SIZE 512

#define HA_DIM_X HUFFMAN_SYMBOLS
#define HA_DIM_Y HUFFMAN_ARRAY_SIZE

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
    Node* tree[HUFFMAN_TOTAL_NODES]; // 512
    Node* nyt;
    char* output;
    char* partial_output;
    int output_length;
    int partial_output_length;
    int elements;
} HuffmanTree;

HuffmanTree* add_new_element(HuffmanTree* ht, char c);
HuffmanTree* createHuffmanTree();
void freeHuffman(HuffmanTree* ht);
Node* find_node(Node *root, int c);
Node* createNYT(int i);
Node* createNode(int node_number, int weight, int element, Node* left, Node* right, Node* parent);
Node* findNYT(Node* root);
int isNYT(Node *pNode);
int isInternalNode(Node *pNode);
Node* check_move(Node* root, Node* node);
void check_move_weight(Node* root, Node* node);
Node* last_of_weight(Node* root, int wtc, int* last);
void swap_nodes(HuffmanTree* ht, Node* node, Node* node2);
void update_weights(Node* root, Node* start);
void* siblings(HuffmanTree* ht, int level, int* size);
void swap_nodes_array(HuffmanTree* ht, int pos, int pos2);
void swapping_array_recursive(HuffmanTree* ht, int pos, int pos2);
void swap_on_diff_lvls(HuffmanTree* ht, Node* node, Node* node2);
void create_subtree_from_node(HuffmanTree *ht, Node *node, Node** result, int* pos);
void generateHTArrayFromTree(HuffmanTree* ht);
char* node_path(Node* node);

void endHuffman(HuffmanTree* ht);