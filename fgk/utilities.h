//
// Created by dvitali on 22.11.17.
//

#ifndef ALGORITMI_FGK_COMPRESSION_UTILITIES_H
#define ALGORITMI_FGK_COMPRESSION_UTILITIES_H

#endif //ALGORITMI_FGK_COMPRESSION_UTILITIES_H

#ifndef ALGORITMI_FGK_COMPRESSION_HUFFMANTREE_H
    #include "huffmantree.h"
#endif

void printTree(Node* root, int level);
void printElement(Node* root);
void printHuffmanTree(HuffmanTree *ht);
void printHuffmanTreeInfo(HuffmanTree *ht);
void printHuffmanArray(HuffmanTree* ht);
int getNodeLevel(Node* node);
int getNodePosition(HuffmanTree* ht, Node* node);