#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>

#include "minunit.h"
#include "colors.h"

#include "defines.h"
#include "console.h"
#include "fgk/huffmantree.h"
#include "fgk/utilities.h"

int tests_run = 0;

void version(){
    printf("%sVIZ compressor %sv%s (%s%s)\n",
           STYLE_BOLD, STYLE_NO_BOLD, VERSION, (RELEASE && !DEBUG? "R-" : (DEBUG ? "D-" : "") ), GIT_VERSION);
}

void usage(){
    version();
#if INVERTED_COMPRESSION_FLAG
    // ... ¯\_(ツ)_/¯
	printf("Compress: \t viz -c inputfile output.viz\n");
#else
	printf("Compress: \t viz -c output.viz inputfile\n");
#endif
	printf("Extract: \t viz -d input.viz\n");
}


/* Node Functions */

#if TEST == 1

static char * test_debug(){
    mu_tag("Debug");
	mu_assert("Error, DEBUG is not enabled", TEST==1);
	return 0;
}

static char * test_get_level(){
    mu_tag("Get Level from pos");
    mu_assert("getLevel(0) is not 0", getLevel(0)==0);
    mu_assert("getLevel(1) is not 1", getLevel(1)==1);
    mu_assert("getLevel(2) is not 1", getLevel(2)==1);
    mu_assert("getLevel(3) is not 2", getLevel(3)==2);
    mu_assert("getLevel(4) is not 2", getLevel(4)==2);
    mu_assert("getLevel(5) is not 2", getLevel(5)==2);
    mu_assert("getLevel(6) is not 2", getLevel(6)==2);
    mu_assert("getLevel(7) is not 3", getLevel(7)==3);
    return 0;
}

static char * test_create_huffman_tree(){
    mu_tag("Huffman Tree Creation");
	HuffmanTree* ht = create_huffman_tree();
    mu_assert("HT has no NYT", ht->nyt != NULL);
    mu_assert("Tree is NULL!", ht->tree != NULL);
    mu_assert("HT doesn't have a root!", ht->root != NULL);
    free_huffman(ht);
	return 0;
}

static char* test_simple_swap(){
    // Test getNodeLevel && getNodePosition on an empty HT
    mu_tag("Simple Swap");

    HuffmanTree* ht = create_huffman_tree();
    free(ht->root);
    ht->root = create_node(511, 1, -1, NULL, NULL, NULL);
    ht->tree[0] = ht->root;

    Node* original_left = create_node(509, 0, NYT_ELEMENT, NULL, NULL, ht->root);
    Node* original_right = create_node(510, 1, 'a', NULL, NULL, ht->root);

    ht->root->left = original_left;
    ht->root->right = original_right;

    mu_assert("Root isn't NN 511", ht->root->node_number == 511);
    mu_assert("Root left is null", ht->root->left != NULL);
    mu_assert("Root left isn't NN 509", ht->root->left->node_number == 509);
    mu_assert("Root left isn't original_left", ht->root->left == original_left);
    mu_assert("Root right is null", ht->root->right != NULL);
    mu_assert("Root right isn't original_right", ht->root->right == original_right);
    mu_assert("Root right isn't NN 510", ht->root->right->node_number == 510);

    printTree(ht->root,0);
    swap_nodes(ht, ht->root->left, ht->root->right);
    printTree(ht->root,0);

    mu_assert("Root isn't NN 511", ht->root->node_number == 511);
    mu_assert("Root left is null", ht->root->left != NULL);
    mu_assert("Root left isn't NN 509", ht->root->left->node_number == 509);
    mu_assert("Root left isn't original_right", ht->root->left == original_right);
    mu_assert("Root left isn't NN 509", ht->root->left->node_number == 509);
    mu_assert("Root right is null", ht->root->right != NULL);
    mu_assert("Root right isn't NN 510", ht->root->right->node_number == 510);
    mu_assert("Root right isn't original_left", ht->root->left == original_right);

    free_huffman(ht);
    return 0;
}


static char * test_huffman_coding(){
    mu_tag("Huffman Coding");
    HuffmanTree* ht = create_huffman_tree();

    mu_assert("Root is not the NYT", ht->root == ht->nyt);
    mu_assert("Root is not the NYT", is_nyt(ht->root));
    mu_assert("Root NN is not 511", ht->root->node_number == HUFFMAN_ARRAY_SIZE-1);

    // Encode "aardvark"
    add_new_element(ht, 'a');

    mu_assert("Root is the NYT", !is_nyt(ht->root));
    mu_assert("Root is not an internal node", is_internal_node(ht->root));
    mu_assert("Root -> Left doesn't exist", ht->root->left != NULL);
    mu_assert("Root -> Left is not the NYT", is_nyt(ht->root->left));
    mu_assert("Root -> Right doesn't exist", ht->root->right != NULL);
    mu_assert("Root -> Right isn't NN 512", ht->root->right->node_number == HUFFMAN_ARRAY_SIZE - 2);
    mu_assert("Root -> Right element isn't 'a'", ht->root->right->element == 'a');
    mu_assert("Root -> Right doesn't have weight 1", ht->root->right->weight == 1);

    add_new_element(ht, 'a');

    mu_assert("Root is the NYT", !is_nyt(ht->root));
    mu_assert("Root is not an internal node", is_internal_node(ht->root));
    mu_assert("Root doesn't have weight 2", ht->root->weight == 2);
    mu_assert("Root -> Left doesn't exist", ht->root->left != NULL);
    mu_assert("Root -> Left is not the NYT", is_nyt(ht->root->left));
    mu_assert("Root -> Right doesn't exist", ht->root->right != NULL);
    mu_assert("Root -> Right isn't NN 512", ht->root->right->node_number == HUFFMAN_ARRAY_SIZE - 2);
    mu_assert("Root -> Right element isn't 'a'", ht->root->right->element == 'a');
    mu_assert("Root -> Right doesn't have weight 1", ht->root->right->weight == 2);

    add_new_element(ht, 'r');
    mu_assert("Root is the NYT", !is_nyt(ht->root));
    mu_assert("Root is not an internal node", is_internal_node(ht->root));
    mu_assert("Root doesn't have weight 3", ht->root->weight == 3);
    mu_assert("Root -> Left doesn't exist", ht->root->left != NULL);
    mu_assert("Root -> Left is not an internal node", is_internal_node(ht->root->left));
    mu_assert("Root -> Left doesn't have an element on its left", ht->root->left->left != NULL);
    mu_assert("Root -> Left doesn't have an element on its right", ht->root->left->right != NULL);
    mu_assert("Root -> Left -> Left isn't the NYT", is_nyt(ht->root->left->left));
    mu_assert("Root -> Left -> Right is an internal node", !is_internal_node(ht->root->left->right));
    mu_assert("Root -> Left -> Right isn't 'r'", ht->root->left->right->element == 'r');
    mu_assert("Root -> Left -> Right doesn't have weight 1", ht->root->left->right->weight == 1);
    mu_assert("Root -> Right doesn't exist", ht->root->right != NULL);
    mu_assert("Root -> Right isn't NN 512", ht->root->right->node_number == HUFFMAN_ARRAY_SIZE - 2);
    mu_assert("Root -> Right element isn't 'a'", ht->root->right->element == 'a');
    mu_assert("Root -> Right doesn't have weight 1", ht->root->right->weight == 2);

    add_new_element(ht, 'd');
    mu_assert("Root is the NYT", !is_nyt(ht->root));
    mu_assert("Root is not an internal node", is_internal_node(ht->root));
    mu_assert("Root doesn't have weight 4", ht->root->weight == 4);
    mu_assert("Root -> Left doesn't exist", ht->root->left != NULL);
    mu_assert("Root -> Left is not an internal node", is_internal_node(ht->root->left));
    mu_assert("Root -> Left doesn't have an element on its left", ht->root->left->left != NULL);
    mu_assert("Root -> Left doesn't have an element on its right", ht->root->left->right != NULL);
    mu_assert("Root -> Left -> Left isn't an internal node", is_internal_node(ht->root->left->left));
    mu_assert("Root -> Left -> Right is an internal node", !is_internal_node(ht->root->left->right));
    mu_assert("Root -> Left -> Right isn't 'r'", ht->root->left->right->element == 'r');
    mu_assert("Root -> Left -> Right doesn't have weight 1", ht->root->left->right->weight == 1);

    mu_assert("Root -> Left -> Left -> Left doesn't exists", ht->root->left->left->left != NULL);
    mu_assert("Root -> Left -> Left -> Left isn't the NYT", is_nyt(ht->root->left->left->left));
    mu_assert("Root -> Left -> Left -> Right doesn't exists", ht->root->left->left->right != NULL);
    mu_assert("Root -> Left -> Left -> Right is an internal node", !is_internal_node(ht->root->left->left->right));
    mu_assert("Root -> Left -> Left -> Right isn't 'd'", ht->root->left->left->right->element == 'd');

    mu_assert("Root -> Right doesn't exist", ht->root->right != NULL);
    mu_assert("Root -> Right isn't NN 512", ht->root->right->node_number == HUFFMAN_ARRAY_SIZE - 2);
    mu_assert("Root -> Right element isn't 'a'", ht->root->right->element == 'a');
    mu_assert("Root -> Right doesn't have weight 1", ht->root->right->weight == 2);

    add_new_element(ht, 'v');
    mu_assert("Root is the NYT", !is_nyt(ht->root));
    mu_assert("Root is not an internal node", is_internal_node(ht->root));
    mu_assert("Root doesn't have weight 5", ht->root->weight == 5);
    mu_assert("Root -> Left doesn't exist", ht->root->left != NULL);
    mu_assert("Root -> Left is an internal node", !is_internal_node(ht->root->left));
    mu_assert("Root -> Left isn't 'a'", ht->root->left->element == 'a');

    mu_assert("Root -> Right doesn't exist", ht->root->right != NULL);
    mu_assert("Root -> Right is not an internal node", is_internal_node(ht->root->right));
    mu_assert("Root -> Right doesn't have weight 3", ht->root->right->weight==3);

    mu_assert("Root -> Right -> Left doesn't exist", ht->root->right->left != NULL);
    mu_assert("Root -> Right -> Left is an internal node", !is_internal_node(ht->root->right->left));
    mu_assert("Root -> Right -> Left isn't an 'r'", ht->root->right->left->element == 'r');
    mu_assert("Root -> Right -> Left doesn't have weight 1", ht->root->right->left->weight == 1);

    mu_assert("Root -> Right -> Right doesn't exist", ht->root->right->right != NULL);
    mu_assert("Root -> Right -> Right isn't an internal node", is_internal_node(ht->root->right->right));
    mu_assert("Root -> Right -> Right doesn't have weight 2", ht->root->right->right->weight == 2);

    mu_assert("Root -> Right -> Right -> Left doesn't exist", ht->root->right->right->left != NULL);
    mu_assert("Root -> Right -> Right -> Left isn't an internal node", is_internal_node(ht->root->right->right->left));
    mu_assert("Root -> Right -> Right -> Left doesn't have weight 1", ht->root->right->right->left->weight == 1);

    mu_assert("Root -> Right -> Right -> Left -> Left doesn't exist", ht->root->right->right->left->left != NULL);
    mu_assert("Root -> Right -> Right -> Left -> Left isn't the NYT", is_nyt(ht->root->right->right->left->left));

    mu_assert("Root -> Right -> Right -> Left -> Right is an internal node", !is_internal_node(ht->root->right->right->left->right));
    mu_assert("Root -> Right -> Right -> Left -> Right isn't 'v'", ht->root->right->right->left->right->element == 'v');
    mu_assert("Root -> Right -> Right -> Left -> Right doesn't have weight 1", ht->root->right->right->left->right->weight == 1);

    mu_assert("Root -> Right -> Right -> Right doesn't exist", ht->root->right->right->right != NULL);
    mu_assert("Root -> Right -> Right -> Right is an internal node", !is_internal_node(ht->root->right->right->right));
    mu_assert("Root -> Right -> Right -> Right isn't 'd'", ht->root->right->right->right->element == 'd');
    mu_assert("Root -> Right -> Right -> Right doesn't have weight 1", ht->root->right->right->right->weight == 1);

    printHuffmanTree(ht);
    free_huffman(ht);

//    printHuffmanTree(ht);
//    printf("Step 1\n");
//    add_new_element(ht, 'A');
//
//    mu_assert("Root doesn't have weight 1", ht->root->weight == 1);
//    mu_assert("Root -> Left is not the NYT", is_nyt(ht->root->left));
//    mu_assert("Root -> Right is a NYT", !is_nyt(ht->root->right));
//    mu_assert("Root -> Right is NULL", ht->root->right != NULL);
//    mu_assert("Root -> Right doesn't have 1 as a weight", ht->root->right->weight == 1);
//    mu_assert("Root -> Right isn't an 'A'", ht->root->right->element == 'A');
//    mu_assert("Root -> Right NN is not 510", ht->root->right->node_number == 510);
//
//    printHuffmanTree(ht);
//    printf("Step 2\n");
//    add_new_element(ht, 'B');
//
//    mu_assert("Root doesn't have weight 2", ht->root->weight == 2);
//    mu_assert("Root -> Left doesn't exist", ht->root->left != NULL);
//    mu_assert("Root -> Left doesn't have weight 1", ht->root->left->weight == 1);
//    mu_assert("Root -> Left isn't the letter 'A'", ht->root->left->element == 'A');
//    mu_assert("Root -> Left doesn't have NN 509", ht->root->left->node_number == 509);
//    mu_assert("Root -> Right doesn't exist", ht->root->right != NULL);
//    mu_assert("Root -> Right doesn't have weight 1", ht->root->right->weight == 1);
//    mu_assert("Root -> Right contains a character (-1 needed!)", ht->root->right->element == -1);
//    mu_assert("Root -> Right doesn't have NN 510", ht->root->right->node_number == 510);
//
//    printf("Root %p (%d), Root => Right: %p (%d)\n", ht->root, ht->root->node_number, ht->root->right, ht->root->right->node_number);
//
//    mu_assert("Root -> Right -> Left doesn't exist", ht->root->right->left != NULL);
//    mu_assert("Root -> Right -> Left is not the NYT", is_nyt(ht->root->right->left));
//    mu_assert("Root -> Right -> Left doesn't have NN 507", ht->root->right->left->node_number == 507);
//    mu_assert("Root -> Right -> Right doesn't exist", ht->root->right->right != NULL);
//    mu_assert("Root -> Right -> Right doesn't have weight 1", ht->root->right->right->weight == 1);
//    mu_assert("Root -> Right -> Right isn't a 'B'", ht->root->right->right->element == 'B');
//    mu_assert("Root -> Right -> Right doesn't have NN 508", ht->root->right->right->node_number == 508);
//
//    printHuffmanTree(ht);
//    printf("%sStep 3%s\n", STYLE_BOLD, STYLE_NO_BOLD);
//    add_new_element(ht, 'C');
//
//    printf("Printing tree\n");
//    printHuffmanTree(ht);
//
//    mu_assert("Root doesn't have weight 3", ht->root->weight == 3);
//    /*mu_assert("Root -> Left doesn't exist", ht->root->left != NULL);
//    mu_assert("Root -> Left doesn't have weight 1", ht->root->left->weight == 1);
//    mu_assert("Root -> Left isn't the letter 'A'", ht->root->left->element == 'A');
//    mu_assert("Root -> Left doesn't have NN 509", ht->root->left->node_number == 509);
//    mu_assert("Root -> Right doesn't exist", ht->root->right != NULL);
//    mu_assert("Root -> Right doesn't have weight 1", ht->root->right->weight == 1);
//    mu_assert("Root -> Right contains a character (-1 needed!)", ht->root->right->element == -1);
//    mu_assert("Root -> Right doesn't have NN 510", ht->root->right->node_number == 510);*/

    return 0;
}

int match_path_string(Node* node, char* string){
    int* length = malloc(sizeof(int));
    unsigned short int* path = node_path(node, length);
    char* path_string = path_to_string(path, *length);
    printf("Node path: %s\n", path_string);

    int result = strcmp(string, path_string) == 0;

    free(path);
    free(path_string);
    free(length);

    return result;
}

static char* test_node_path(){
    mu_tag("Node Path");
    Node* root = create_node(511, 4, -1, NULL, NULL, NULL);
    Node* two_one = create_node(509, 2, -1, NULL, NULL, root);
    Node* two_two = create_node(510, 2, 'B', NULL, NULL, root);

    root->left = two_one;
    root->right = two_two;

    Node* three_one = create_node(507, 1, -1, NULL, NULL, two_one);
    Node* three_two = create_node(508, 1, 'A', NULL, NULL, two_one);

    two_one->left = three_one;
    two_one->right = three_two;

    Node* four_one = create_node(505, 1, 'C', NULL, NULL, three_one);
    Node* four_two = create_node(506, 1, 'C', NULL, NULL, three_one);

    three_one->left = four_one;
    three_one->right = four_two;



    mu_assert("Node path is not 00", match_path_string(three_one, "00"));
    mu_assert("Node path is not 01", match_path_string(three_two, "01"));
    mu_assert("Node path is not 000", match_path_string(four_one, "000"));
    mu_assert("Node path is not 001", match_path_string(four_two, "001"));

    // Free nodes
    free(root);
    free(two_one);
    free(two_two);
    free(three_one);
    free(three_two);
    free(four_one);
    free(four_two);

    return 0;
}

static char* test_swap_nodes(){
    mu_tag("Swap Nodes");

    HuffmanTree* ht = create_huffman_tree();
    Node* root = create_node(511, 4, -1, NULL, NULL, NULL);
    Node* two_one = create_node(509, 2, -1, NULL, NULL, root);
    Node* two_two = create_node(510, 2, 'B', NULL, NULL, root);

    root->left = two_one;
    root->right = two_two;

    Node* three_one = create_node(507, 1, -1, NULL, NULL, two_one);
    Node* three_two = create_node(508, 1, 'A', NULL, NULL, two_one);

    two_one->left = three_one;
    two_one->right = three_two;

    Node* four_one = create_nyt(505);
    Node* four_two = create_node(506, 1, 'C', NULL, NULL, three_one);

    three_one->left = four_one;
    three_one->right = four_two;

    ht->nyt = four_one;
    free(ht->root);
    ht->root = root;


    printf("\n");
    /*
    ht->tree[0] = root;
    ht->tree[1] = two_one;
    ht->tree[2] = two_two;
    ht->tree[3] = three_one;
    ht->tree[4] = three_two;
    // 5 = NULL
    // 6 = NULL
    ht->tree[7] = four_one;
    ht->tree[8] = four_two;*/


    printHuffmanArray(ht);
    printHuffmanTree(ht);


    // Swapping 509 w 510

    Node* first_p = ht->root->left;
    Node first = *first_p;
    Node* second_p = ht->root->right;
    Node second = *second_p;

    saveHuffmanTree(ht, "./test/results/t_sn_1.dot");

    debug("Swap: 1/3");
    swap_nodes(ht, ht->root->left, ht->root->right);

    printHuffmanTree(ht);

    mu_assert("Root -> Left doesn't exist", ht->root->left != NULL);
    mu_assert("Root -> Left weight is incorrect", ht->root->left->weight == second.weight);
    mu_assert("Root -> Left NN is incorrect", ht->root->left->node_number == 509);
    mu_assert("Root -> Left element is incorrect", ht->root->left->element == second.element);
    mu_assert("Root -> Left -> Left is incorrect", ht->root->left->left == second.left);
    mu_assert("Root -> Left -> Right is incorrect", ht->root->left->right == second.right);

    mu_assert("Root -> Right doesn't exist", ht->root->right != NULL);
    mu_assert("Root -> Right weight is incorrect", ht->root->right->weight == first.weight);
    mu_assert("Root -> Right NN is incorrect", ht->root->right->node_number == 510);
    mu_assert("Root -> Right element is incorrect", ht->root->right->element == first.element);
    mu_assert("Root -> Right -> Left is incorrect", ht->root->right->left == first.left);
    mu_assert("Root -> Right -> Right is incorrect", ht->root->right->right == first.right);

    saveHuffmanTree(ht, "./test/results/t_sn_2.dot");

    // Test swap back

    debug("Swap: 2/3");
    swap_nodes(ht, ht->root->left, ht->root->right);
    saveHuffmanTree(ht, "./test/results/t_sn_3.dot");


    debug("Swap: 3/3");
    // Swap 507 w/ 510
    swap_nodes(ht, ht->root->left->left, ht->root->right);
    saveHuffmanTree(ht, "./test/results/t_sn_4.dot");

    debug("[Swap Test] Printing Huffman Tree");
    printHuffmanTree(ht);
    printf("[Swap Test] Printing swapped HT Array:\n");
    printHuffmanArray(ht);
    debug("[Swap Test] Printing generated array");
    printHuffmanArray(ht);
    printf("\n");
    /*for(i = 0; i < sizeof(ht_array);i++){
        printf("%d,",i);
        mu_assert("Invalid ht_array entry.", ht->tree[i] == ht->tree[i]);
    }*/
    printf("\n");

    free_huffman(ht);

    return 0;
}

static char* test_get_node_level(){
    HuffmanTree* ht = create_huffman_tree();
    free_huffman(ht);
    return 0;
}

static char* test_huffman_coding_abracadabra(){
    mu_tag("Huffman Coding (ABRACADABRA)");
    HuffmanTree* ht = create_huffman_tree();
    add_new_element(ht, 'A');
    add_new_element(ht, 'B');
    add_new_element(ht, 'R');
    add_new_element(ht, 'A');
    add_new_element(ht, 'C');
    printHuffmanTree(ht);
    free_huffman(ht);
    return 0;
}

static char* test_huffman_coding_abcbaaa(){
    mu_tag("Huffman Coding (abcbaaa)");
    HuffmanTree* ht = create_huffman_tree();
    add_new_element(ht, 'a');
    printHuffmanTree(ht);
    add_new_element(ht, 'b');
    printHuffmanTree(ht);
    add_new_element(ht, 'c');
    printHuffmanTree(ht);
    add_new_element(ht, 'b');
    printHuffmanTree(ht);
    add_new_element(ht, 'a');
    printHuffmanTree(ht);
    add_new_element(ht, 'a');
    printHuffmanTree(ht);
    add_new_element(ht, 'a');
    printHuffmanTree(ht);
    free_huffman(ht);
    return 0;
}

static char* test_bin2byte(){
    mu_tag("Binary to Byte (w/ padding)");
    int* length = malloc(sizeof(int));
    *length = 0;
    char* result;
    int i;

    /*result = bin2byte("001", length);
    printf("Length: %d\n", *length);
    for(i=0; i<*length; i++){
        printf("result: %02x\n", result[i]);
    }*/

    char result_string[10];

    char* tests[9] = {
            "001",
            "0101",
            "0",
            "1",
            "11",
            "1101",
            "11111111",
            "01111111",
            "11111110"
    };

    char* expected_results[9] = {
            "20",
            "50",
            "00",
            "80",
            "C0",
            "D0",
            "FF",
            "7F",
            "FE"
    };

    int size = (sizeof(expected_results)/sizeof(char*));

    for(i = 0; i<size; i++) {
        printf("Testing entry #%d\n", i);
        printf("Testing w/ %s\n", tests[i]);

        result = bin2byte(tests[i], length);
        sprintf(result_string, "%02X", result[0] & 0xff);
        mu_assert("Length is not 1", *length == 1);
        char error_string[50];

        int test_result = strncmp(result_string, expected_results[i], strlen(expected_results[i]));
        if(test_result != 0){
            sprintf(error_string, "Result is not \"%s\" - was \"%s\"", expected_results[i], result_string);
            printf("%s\n", error_string);
        }
        mu_assert("Value is not the expected result. Check previous output.", test_result == 0);
        free(result);
    }

    free(length);

    return 0;
}

static char* test_byte2bin(){
    mu_tag("Byte2Bin");
    unsigned short* result;
    result = byte2bit('\xff');
    unsigned short* expected_result = (unsigned short[8]){1,1,1,1,1,1,1,1};
    mu_assert("FF is not 11111111", compare_short_int(result, expected_result, 8));
    free(result);

    result = byte2bit('\xfa');
    expected_result = (unsigned short[8]){1,1,1,1,1,0,1,0};
    mu_assert("FA is not 11111010", compare_short_int(result, expected_result, 8));
    free(result);

    result = byte2bit('\x0a');
    expected_result = (unsigned short[8]){0,0,0,0,1,0,1,0};
    mu_assert("0A is not 00001010", compare_short_int(result, expected_result, 8));
    free(result);

    result = byte2bit('\x00');
    expected_result = (unsigned short[8]){0,0,0,0,0,0,0,0};
    mu_assert("00 is not 00000000", compare_short_int(result, expected_result, 8));
    free(result);

    return 0;
}

static char* test_bin2byte2(){
    mu_tag("Binary to byte - 2 byte cases (w/ padding)");
    int* length = malloc(sizeof(int));
    *length = 0;
    char* result;
    int i;

    /*result = bin2byte("001", length);
    printf("Length: %d\n", *length);
    for(i=0; i<*length; i++){
        printf("result: %02x\n", result[i]);
    }*/

    char result_string[10];

    char* tests[7] = {
            "000000001",
            "0110010101",
            "00101000111",
            "01111101000",
            "10101100101010",
            "111111111",
            "000000000"
    };

    char* expected_results[7] = {
            "0080",
            "6540",
            "28E0",
            "7D00",
            "ACA8",
            "FF80",
            "0000"
    };

    int size = (sizeof(expected_results)/sizeof(char*));

    for(i = 0; i<size; i++) {
        printf("Testing entry #%d\n", i);
        printf("Testing w/ %s\n", tests[i]);

        result = bin2byte(tests[i], length);
        sprintf(result_string, "%02X%02X", result[1] & 0xff, result[0] & 0xff);
        mu_assert("Length is not 2", *length == 2);
        char error_string[50];

        int test_result = strncmp(result_string, expected_results[i], strlen(expected_results[i]));
        if(test_result != 0){
            sprintf(error_string, "Result is not \"%s\" - was \"%s\"", expected_results[i], result_string);
            printf("%s\n", error_string);
        }
        mu_assert("Value is not the expected result. Check previous output.", test_result == 0);
        free(result);
    }

    free(length);

    return 0;
}


static char* test_huffman_coding_general(char* string){
    char title[200];
    sprintf(title, "Huffman Coding (%s)", string);
    mu_tag(title);

    HuffmanTree* ht = create_huffman_tree();

    int i;
    for(i=0; i<strlen(string); i++){
        char dbg[50];
        sprintf(dbg, "Adding character %c", string[i]);
        debug(dbg);
        add_new_element(ht, string[i]);
        //sprintf(path, "%s_%d.dot", string, i);
        //saveHuffmanTree(ht, path);
    }

    char * buffer = "";
    long length;
    FILE * fh;

    char filepath[200];
#ifdef UTILITIES_PRINT_CHARS
    debug("UTILITIES PRINT CHARS IS ON");
    sprintf(filepath, "./test/expected-results/%s.txt", string);
    fh = fopen (filepath, "r");

#else
    debug("UTILITIES PRINT CHARS IS OFF");
    sprintf(filepath, "./test/expected-results/%s.hex", string);
    fh = fopen (filepath, "r");
#endif

    //printHuffmanTree(ht);
    if (fh)
    {
        fseek (fh, 0, SEEK_END);
        length = ftell (fh);
        fseek (fh, 0, SEEK_SET);
        buffer = malloc (length);
        if (buffer)
        {
            fread (buffer, 1, length, fh);
        }
        fclose (fh);
    } else {
        mu_assert("File not found", fh!=NULL);
    }
    char* resulting_tree = getTree(ht->root, 0);

    mu_assert("Invalid HT", strncmp(buffer, resulting_tree, strlen(resulting_tree)) == 0);
    free(resulting_tree);
    free(buffer);
    free_huffman(ht);

    return 0;

}


static char* test_huffman_coding_mississippi(){
    return test_huffman_coding_general("mississippi");
}

static char* test_huffman_coding_engineering(){
    return test_huffman_coding_general("engineering");
}

static char* test_huffman_coding_sleeplessness(){
    return test_huffman_coding_general("sleeplessness");
}

static char* test_huffman_coding_aardvark(){
    return test_huffman_coding_general("aardvark");
}

static char* test_huffman_coding_foobar(){
    return test_huffman_coding_general("foobar");
}

static char * test_filename(){
    mu_tag("File name");

    char* values[5] = {
            "some/very/long/path/that/ends/like/this.txt",
            "some/very/long/path/that/ends/like/",
            "file.txt",
            "./file.bin",
            "../a/.././my-name.jpg"
    };

    char* expected_results[5] = {
            "this.txt",
            "",
            "file.txt",
            "file.bin",
            "my-name.jpg"
    };

    int i;
    for(i=0; i<sizeof(values)/sizeof(char*); i++){
        printf("Testing %s...\n", values[i]);
        char buffer[100];
        sprintf(buffer, "'%s' doesn't return '%s'", values[i], expected_results[i]);

        char* result = get_filename(values[i]);

        if(strcmp(result, expected_results[i]) != 0){
            error(buffer);
            mu_assert("test_filaname failed. Check previous output", 0);
        }
        free(result);
    }

    return 0;
}

char* test_create_file(){
    mu_tag("Create file (aka touch)");
    int result = file_touch("./out-test.viz");
    mu_assert("Can't create out.viz", result==1);
    return 0;
}

char * test_write_to_file(){
    mu_tag("Write to file");
    int result = file_write("./out-test.viz", "Test\n");
    mu_assert("Error while writing 'Test' to ./out-test.viz", result==1);
    return 0;
}

char * test_read_file(){
    mu_tag("Read file");
    char* buffer[200];
    int* error = malloc(sizeof(int));
    char* result = file_read("./out-test.viz", error);
    mu_assert("Error while reading from ./out-test.viz", result != NULL);
    mu_assert("File content isn't 'Test\\n'", strncmp(result, "Test\n", 5) == 0);
    free(result);
    free(error);
    return 0;
}

char * test_file_delete(){
    mu_tag("Delete file");
    int result = file_delete("./out-test.viz");
    mu_assert("File cannot be deleted", result == 0);
    return 0;
}

static char * test_huffman_coding_bookkeeper(){
    return test_huffman_coding_general("bookkeeper");
}

static char * all_tests(){
    mu_run_test(test_debug);

    mu_run_test(test_create_huffman_tree);
    //mu_run_test(test_swap_ht_array);

    //mu_run_test(test_huffman_coding_foobar2000);
    //mu_run_test(test_huffman_coding_loremipsum);

    mu_run_test(test_get_level);
    mu_run_test(test_get_node_level);
    mu_run_test(test_simple_swap);
    mu_run_test(test_swap_nodes);
    mu_run_test(test_node_path);
    mu_run_test(test_huffman_coding);
    //mu_run_test(test_utility_siblings);
    mu_run_test(test_huffman_coding);
    mu_run_test(test_huffman_coding_abracadabra);
    mu_run_test(test_huffman_coding_abcbaaa);
    mu_run_test(test_huffman_coding_bookkeeper);
    mu_run_test(test_huffman_coding_mississippi);
    mu_run_test(test_huffman_coding_engineering);
    mu_run_test(test_huffman_coding_foobar);
    mu_run_test(test_huffman_coding_aardvark);
    mu_run_test(test_huffman_coding_sleeplessness);
    mu_run_test(test_bin2byte);
    mu_run_test(test_bin2byte2);
    mu_run_test(test_byte2bin);
    mu_run_test(test_filename);


    // File ops. Run in sequence!
    mu_run_test(test_create_file);
    mu_run_test(test_write_to_file);
    mu_run_test(test_read_file);
    mu_run_test(test_file_delete);

	return 0;
}
#endif

int main(int argc, char *argv[]) {

#if TEST == 1
    char *result = all_tests();
    if (result != 0) {
        //printf("%s\n", result);
        printf("Test failed.\n");
        return 1; // Fail on test failed
    }
    else {
        test_successful("ALL TESTS PASSED");
    }
    printf("Tests run: %d\n", tests_run);
    return 0;
#endif

    debug("VIZ is running in DEBUG mode.");

    char debug_buffer[500];
    if (argc == 1) {
        usage();
        return 0;
    }

    if (strcmp(argv[1], "-v") == 0) {
        version();
        return 0;
    }

    if (DEBUG) {
        int i;
        for (i = 0; i < argc; i++) {
            char buffer[500];
            sprintf(buffer, "Argument %d: %s", i, argv[i]);
            debug(buffer);
        }
    }

    if (strcmp(argv[1], "-t") == 0) {
        // Tree test

        // First level
        Node *root = create_node(11, 32, -1, NULL, NULL, NULL);

        // Second Level
        Node *n_9 = create_node(9, 11, 'f', NULL, NULL, root);
        Node *n_10 = create_node(10, 21, -1, NULL, NULL, root);

        // Third level
        Node *n_7 = create_node(7, 10, -1, NULL, NULL, n_10);
        Node *n_8 = create_node(8, 11, -1, NULL, NULL, n_10);

        // Fourth Level
        Node *n_3 = create_node(3, 5, 'c', NULL, NULL, n_7);
        Node *n_4 = create_node(4, 5, -1, NULL, NULL, n_7);
        Node *n_5 = create_node(5, 5, 'd', NULL, NULL, n_8);
        Node *n_6 = create_node(6, 6, 'e', NULL, NULL, n_8);

        // Fifth Level
        Node *n_1 = create_node(1, 2, 'a', NULL, NULL, n_4);
        Node *n_2 = create_node(2, 3, 'b', NULL, NULL, n_4);

        root->left = n_9;
        root->right = n_10;

        n_10->left = n_7;
        n_10->right = n_8;

        n_7->left = n_3;
        n_7->right = n_4;

        n_4->left = n_1;
        n_4->right = n_2;

        n_8->left = n_5;
        n_8->right = n_6;

        printTree(root, 0);


        return 0;
    }

    if (strcmp(argv[1], "-c") == 0) {
        int flag_overwrite = 0;
        int flag_shift = 0;

        if (strncmp(argv[2], "-", 1) == 0) {
            // Additional flag found
            if (strcmp(argv[2], "-f") == 0) {
                flag_overwrite = 1;
                flag_shift++;
            }
        }
        if (argc != 4 && flag_shift == 0) {
            usage();
            return 1;
        }

        debug("Compression");

#if INVERTED_COMPRESSION_FLAG
        // Come on. Seriously? - D.
        char *file_output = (char *) malloc(strlen(argv[3 + flag_shift]) + 1);
        char *file_input = (char *) malloc(strlen(argv[2 + flag_shift]) + 1);

        char file_output_tmp[strlen(argv[3 + flag_shift]) + 1 + 4]; // output.ext.tmp

        strcpy(file_output, argv[3 + flag_shift]);
        strcpy(file_input, argv[2 + flag_shift]);
#else
        char *file_output = (char *) malloc(strlen(argv[2 + flag_shift]) + 1);
        char *file_input = (char *) malloc(strlen(argv[3 + flag_shift]) + 1);

        char file_output_tmp[strlen(argv[2 + flag_shift]) + 1 + 4]; // output.ext.tmp

        strcpy(file_output, argv[2 + flag_shift]);
        strcpy(file_input, argv[3 + flag_shift]);
#endif

        sprintf(file_output_tmp, "%s.tmp", file_output);

        if (DEBUG) {
            sprintf(debug_buffer, "Input: %s", file_input);
            debug(debug_buffer);
            sprintf(debug_buffer, "Output: %s", file_output);
            debug(debug_buffer);
        }

        // Check if file exists
        struct stat stat_result;
        int result = stat(file_input, &stat_result);

        if (result != 0) {
            // File doesn't exist!
            char buffer[500];
            if (DEBUG) {
                sprintf(buffer, "%s does not exist (E: %d).", file_input, result);
            } else {
                sprintf(buffer, "%s does not exist.", file_input);
            }
            error(buffer);
            return 1;
        }

        result = access(file_output, F_OK);
        if (result == 0 && flag_overwrite) {
            char buffer[500];
            sprintf(buffer, "%s will be overwritten", file_output);
            info(buffer);
        } else if (result == 0 && flag_overwrite == 0) {
            char buffer[500];
            sprintf(buffer, "%s already exists. If you really want to overwrite it, add '-f' to the flags.",
                    file_output);
            error(buffer);
            return 1;
        } else if (result == -1) {
            if (DEBUG) {
                char buffer[500];
                sprintf(buffer, "File %s doesn't exist. This is normal. (E%d)", file_output, result);
                warn(buffer);
            }
        } else {
            char buffer[500];
            sprintf(buffer, "Unable to write %s: permission denied (E: %d).", file_output, result);
            error(buffer);
        }

        result = access(file_output_tmp, W_OK);
        if (result != 0) {

            if (result == -1) {
                // File doesn't exists

            } else {
                // Cannot write file!
                char buffer[500];
                if (DEBUG) {
                    sprintf(buffer, "Unable to write %s: permission denied (E: %d).", file_output_tmp, result);
                } else {
                    sprintf(buffer, "Unable to write %s: permission denied.", file_output_tmp);
                }
                error(buffer);
                warn("WTF");
                return 1;
            }
        }

        // Input exists, Output can be written

        FILE *fh = fopen(file_input, "rb");
        FILE *o_tmp_fh = fopen(file_output_tmp, "wb+");
        FILE *o_fh = fopen(file_output, "wb");

        // We'll eventually switch to a buffer for better performances,
        // for now, we stick to our fgetc function since we're working w/ small files

        // Do the Huffman Coding Thing
        HuffmanTree *ht = create_huffman_tree();

        if (o_tmp_fh == NULL) {
            // The file doesn't exist, create it
            o_tmp_fh = fopen(file_output_tmp, "wb");
        }

        if(o_tmp_fh == NULL){
            char error_msg[200];
            sprintf(error_msg, "Unable to create %s", file_output_tmp);
            error(error_msg);
            exit(1);
        }

        int i;

        size_t buffer_size = 4096;
        char *buffer = calloc(buffer_size, sizeof(char));

        ht->buffer = (int) buffer_size;

        int total_read = 0;

        while (!feof(fh)) {
            if (ferror(fh)) {
                perror("Error was");
                break;
            }

            size_t read = fread(buffer, 1, buffer_size, fh);
            total_read += read;
            for (i = 0; i < read; i++) {
                add_new_element(ht, buffer[i]);
                if(ht->output_length != 0) {
                    /*if(DEBUG) {
                        debug("[main] Output is not 0, adding bytes to file content");
                        printf("Adding %d bytes:\n", ht->output_length);
                        for (int k = 0; k < ht->output_length; k++) {
                            printf("%02x ", ht->output[k] & 0xff);
                        }
                        printf("\n");
                        char append[20];
                        sprintf(append, "debug_%d.dot", i);
                        saveHuffmanTree(ht, append);
                    }*/
                    fwrite(ht->output, sizeof(char), (size_t) ht->output_length, o_tmp_fh);
                    ht->output_length = 0;
                }
            }
        }

        if(total_read != 0) {
            endHuffman(ht);
        }

        if(ht->output_length != 0){
            fwrite(ht->output, sizeof(char), (size_t) ht->output_length, o_tmp_fh);
        }

        int compress = 0;

        fseek(o_tmp_fh, 0L, SEEK_END);
        long size = ftell(o_tmp_fh);

        fseek(fh, 0L, SEEK_END);
        long original_size = ftell(fh);


        rewind(fh);
        rewind(o_tmp_fh);

        char *file_name = get_filename(file_input);

        fprintf(o_fh, "%s", MAGIC_NUMBER);

        if (size < original_size || FORCE_COMPRESSION) {
            compress = 1;
        }

        if (compress == 0) {
            fprintf(o_fh, "%s", "\x01"); // NOT compressed!
        } else {
            fprintf(o_fh, "%s", "\x02"); // Compressed!
        }
        fprintf(o_fh, "%s", file_name); // Extracted file name
        fprintf(o_fh, "%s", "\x02"); // Start of compressed file

        if (compress == 0) {
            info("The file won't be compressed because its size exceeds the original one");
            while (!feof(fh)) {
                if (ferror(o_tmp_fh)) {
                    perror("Unable to process the file");
                    break;
                }
                size_t size_read = fread(buffer, 1, (size_t) buffer_size, fh);
                fwrite(buffer, 1, size_read, o_fh);
            }
        } else {
            info("The file was compressed successfully");
            while (!feof(o_tmp_fh)) {
                if (ferror(o_tmp_fh)) {
                    perror("Unable to process the file");
                    break;
                }
                size_t size_read = fread(buffer, 1, (size_t) buffer_size, o_tmp_fh);
                fwrite(buffer, 1, size_read, o_fh);
            }
        }

        if (!DEBUG) {
            // Debug mode keeps the .tmp file for future analysis
            file_delete(file_output_tmp);
        }

        free(file_name);

        printf("\n");
        free_huffman(ht);
        fclose(fh);
        fclose(o_fh);
        fclose(o_tmp_fh);

        free(buffer);
        free(file_input);
        free(file_output);

        return 0;
    }

    if (strcmp(argv[1], "-d") == 0) {
        // Decompression

        if (argc != 3) {
            usage();
            return 1;
        }

        debug("Decompressing");
        char *file_input = (char *) malloc(strlen(argv[2]) + 1);
        strcpy(file_input, argv[2]);


        int result = access(file_input, F_OK);
        if (result == -1) {
            if (DEBUG) {
                char buffer[500];
                sprintf(buffer, "File %s doesn't exist. (E%d)", file_input, result);
            } else {
                char buffer[500];
                sprintf(buffer, "File %s doesn't exist.", file_input);
                error(buffer);
            }
        }

        //info("Decompressing your file...");

        FILE *fh = fopen(file_input, "rb");

        if(!fh){
            char buffer[500];
            sprintf(buffer, "File %s doesn't exist.", file_input);
            error(buffer);
            exit(1);
        }

        // Parse headers
        size_t buffer_size = 50;
        char *buffer = calloc(buffer_size, sizeof(char));

        size_t read_bytes;
        read_bytes = fread(buffer, 1, 5, fh);

        if (read_bytes != 5) {
            error("Invalid file.");
            return 1;
        }

        int header_size = 5;

        if (strncmp(MAGIC_NUMBER, buffer, strlen(MAGIC_NUMBER)) != 0) {
            error("The provided file is not compatible because it wasn't generated with VIZ.");
            return 1;
        }

        int compressed = 0;

        if (buffer[strlen(MAGIC_NUMBER)] == '\x01') {
            compressed = 0;
        } else if (buffer[strlen(MAGIC_NUMBER)] == '\x02') {
            compressed = 1;
        } else {
            error("The file provided is invalid or corrupted.");
            return 1;
        }

        char info_buffer[50];
        sprintf(info_buffer, "The file was%scompressed", (!compressed ? " not " : " "));
        info(info_buffer);

        int c;

        char filename[200];
        int i = 0;

        while(EOF != (c = fgetc(fh))){
            if(c == '\x02'){
                break;
            }
            filename[i] = (char) c;
            i++;
        }

        header_size += i;

        filename[i] = '\0';

        FILE *o_fh = fopen(filename, "wb+");
        if(!o_fh){
            // Unable to create filename
            error("Unable to extract this file. Please check that you have write permissions in this folder.");
            exit(1);
        }

        int b_size = 4096;
        char read_buffer[b_size];
        char write_buffer[b_size*8];
        //fseek(fh, header_size, SEEK_SET);
        size_t read_size = 0;

        HuffmanTree* ht = create_huffman_tree();
        ht->mode = H_MODE_DECOMPRESSOR;
        ht->buffer = b_size;

        int k;
        int written_bytes = 0;

        if(ferror(fh)){
            error("Input file error!");
            exit(3);
        }

        read_size = fread(read_buffer,sizeof(char), (size_t) b_size, fh);

        while(read_size != 0){
            if(compressed) {
                for (i = 0; i < read_size; i++) {
                    ht->partial_output[ht->partial_output_length] = read_buffer[i];
                    ht->partial_output_length++;
                }

                ht->decoder_last_chunk = (unsigned short) (read_size < b_size);
#if DEBUG
                if (ht->decoder_last_chunk) {
                    printf("%d Last chunk.\n", (int) read_size);
                }
#endif
                while (decode_byte(ht) != 0) {

#if DEBUG
                    printf("Decoding byte %d/%d (0x%02x)\n", i, (int) read_size,
                           ht->partial_output[ht->decoder_byte] & 0xff);
#endif
                    int wb = 0;
                    for (k = 0; k < ht->output_length; k++) {
#if DEBUG
                        printf("wb + k: %d\n", written_bytes + k);
                        printf("[Decompressor] Character: 0x%02x\n", ht->output[k]&0xff);
#endif
                        write_buffer[written_bytes + k] = ht->output[k];
                        wb++;
                    }
                    written_bytes += wb;
                    free(ht->output);
                    ht->output = calloc(256, sizeof(char));
                    ht->output_length = 0;

                }

                fwrite(write_buffer, sizeof(char), (size_t) written_bytes, o_fh);
                huffman_shift_partial_output(ht, ht->decoder_byte);
                ht->decoder_byte = 0;

                written_bytes = 0;
            } else {
                fwrite(read_buffer, sizeof(char), (size_t) read_size, o_fh);
            }

            read_size = fread(read_buffer,sizeof(char), (size_t) b_size, fh);
        }

        free_huffman(ht);
        free(file_input);

        fclose(fh);
        fclose(o_fh);



        free(buffer);

    }
}
