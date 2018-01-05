//
// Created by dvitali on 22.11.17.
//

#ifndef ALGORITMI_FGK_COMPRESSION_UTILITIES_H
#define ALGORITMI_FGK_COMPRESSION_UTILITIES_H

#endif //ALGORITMI_FGK_COMPRESSION_UTILITIES_H

#ifndef ALGORITMI_FGK_COMPRESSION_HUFFMANTREE_H
    #include "huffmantree.h"
#endif

char* getTree(Node* root, int level);
void printTree(Node* root, int level);


char* getElement(Node* root);
void printElement(Node* root);

void printHuffmanTree(HuffmanTree *ht);
void saveHuffmanTree(HuffmanTree *ht, char* filename);

void printHuffmanTreeInfo(HuffmanTree *ht);
void printHuffmanArray(HuffmanTree* ht);

void printNodeArray(Node** array);
int getNodeLevel(Node* node);
int* getNodePosition(HuffmanTree* ht, Node* node);

int getLevel(int pos);
Node *getParent(HuffmanTree *ht, Node *pNode);
void printPartialArray(Node** arr);

// Byte / Bit manipulation
char* bin2byte(char* bin, int* length);
char* byte2bit(char b);

// File functions
char* get_filename(char* string);
int file_touch(char* path);
int file_write(char* path, char* content);
char* file_read(char* path, int* error);
int file_delete(char* path);