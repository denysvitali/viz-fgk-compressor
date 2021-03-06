#ifndef ALGORITMI_FGK_COMPRESSION_HUFFMANTREE_H
#define ALGORITMI_FGK_COMPRESSION_HUFFMANTREE_H

#endif //ALGORITMI_FGK_COMPRESSION_HUFFMANTREE_H

#define HUFFMAN_SYMBOLS 257
#define HUFFMAN_ARRAY_SIZE (514 + 1)

#define H_DECODER_FLAG_NEXT_IS_BYTE 1

#define H_MODE_COMPRESSOR 0
#define H_MODE_DECOMPRESSOR 1

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
    Node* tree[HUFFMAN_ARRAY_SIZE]; // 514
    Node* element_array[HUFFMAN_SYMBOLS];
    Node* nyt;

    char* output;
    int output_length;

    char* partial_output;
    int partial_output_length;

    int elements;
    unsigned int mode;
    unsigned char mask;
    unsigned short decoder_last_chunk;
    int decoder_byte;
    int buffer;
} HuffmanTree;

HuffmanTree* add_new_element(HuffmanTree* ht, char c);
HuffmanTree* create_huffman_tree();
void free_huffman(HuffmanTree *ht);
Node* find_node(HuffmanTree* ht, int c);
Node* create_nyt(int i);
Node* create_node(int node_number, int weight, int element, Node *left, Node *right, Node *parent);
Node* find_nyt(Node *root);
int is_nyt(Node *pNode);
int is_internal_node(Node *pNode);
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
unsigned short* node_path(Node* node, int* length);
int is_leaf(Node* pNode);
unsigned int get_bit(HuffmanTree* ht);



void endHuffman(HuffmanTree* ht);

// Decompressor
int decode_byte(HuffmanTree* ht);
void huffman_shift_partial_output(HuffmanTree* ht, int byte);
