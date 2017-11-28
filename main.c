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

void usage(){
	printf("%sVIZ compressor %sv%s (%s)\n", STYLE_BOLD, STYLE_NO_BOLD, VERSION, GIT_VERSION);
	printf("Compress: \t viz -c output.viz inputfile\n");
	printf("Extract: \t viz -d input.viz\n");
}


/* Node Functions */

#if TEST == 1

static char * test_foo(){
	mu_assert("Error, DEBUG is not enabled", TEST==1);
	return 0;
}

static char * test_create_huffman_tree(){
	HuffmanTree* ht = createHuffmanTree();
    mu_assert("HT has no NYT", ht->nyt != NULL);
    mu_assert("Tree is NULL!", ht->tree != NULL);
    mu_assert("HT doesn't have a root!", ht->root != NULL);
	return 0;
}

static char * test_last_of_weight(){
    HuffmanTree* ht = createHuffmanTree();
    Node* root = createNode(511, 4, -1, NULL, NULL, NULL);
    Node* two_one = createNode(509, 2, -1, NULL, NULL, root);
    Node* two_two = createNode(510, 2, 'B', NULL, NULL, root);

    root->left = two_one;
    root->right = two_two;

    Node* three_one = createNode(507, 1, -1, NULL, NULL, two_one);
    Node* three_two = createNode(508, 1, 'A', NULL, NULL, two_one);

    two_one->left = three_one;
    two_one->right = three_two;

    Node* four_one = createNYT(505);
    Node* four_two = createNode(506, 1, 'C', NULL, NULL, three_one);

    three_one->left = four_one;
    three_one->right = four_two;

    ht->nyt = four_one;

    ht->root = root;

    printf("Root L: %p\n", root->left);
    printf("Root D: %p\n", root->right);

    printHuffmanTree(ht);

    int a = 0;
    Node* node = last_of_weight(ht->root, 2, &a);

    mu_assert("Last of Weight returned NULL", node != NULL);
    printf("A: %d\n", a);
    printf("Node: %p\n\n", node);
    mu_assert("Last Of Weight is not correct", a == 510);
    mu_assert("Node Number isn't equal to pointed value", node->node_number == a);
    return 0;
}

static char * test_create_ht_array(){
    HuffmanTree* ht = createHuffmanTree();
    mu_assert("HT.TREE[0] is not NYT / Root", ht->tree[0] == ht->nyt && ht->tree[0] == ht->root);
    return 0;
}

static char * test_huffman_coding(){
    HuffmanTree* ht = createHuffmanTree();
    printHuffmanTree(ht);
    printf("Step 1\n");
    add_new_element(ht, 'A');

    mu_assert("Root doesn't have weight 1", ht->root->weight == 1);
    mu_assert("Root -> Left is not the NYT", isNYT(ht->root->left));
    mu_assert("Root -> Right is a NYT", !isNYT(ht->root->right));
    mu_assert("Root -> Right is NULL", ht->root->right != NULL);
    mu_assert("Root -> Right doesn't have 1 as a weight", ht->root->right->weight == 1);
    mu_assert("Root -> Right isn't an 'A'", ht->root->right->element == 'A');
    mu_assert("Root -> Right NN is not 510", ht->root->right->node_number == 510);

    printHuffmanTree(ht);
    printf("Step 2\n");
    add_new_element(ht, 'B');

    mu_assert("Root doesn't have weight 2", ht->root->weight == 2);
    mu_assert("Root -> Left doesn't exist", ht->root->left != NULL);
    mu_assert("Root -> Left doesn't have weight 1", ht->root->left->weight == 1);
    mu_assert("Root -> Left isn't the letter 'A'", ht->root->left->element == 'A');
    mu_assert("Root -> Left doesn't have NN 509", ht->root->left->node_number == 509);
    mu_assert("Root -> Right doesn't exist", ht->root->right != NULL);
    mu_assert("Root -> Right doesn't have weight 1", ht->root->right->weight == 1);
    mu_assert("Root -> Right contains a character (-1 needed!)", ht->root->right->element == -1);
    mu_assert("Root -> Right doesn't have NN 510", ht->root->right->node_number == 510);

    printf("Root %p (%d), Root => Right: %p (%d)\n", ht->root, ht->root->node_number, ht->root->right, ht->root->right->node_number);

    mu_assert("Root -> Right -> Left doesn't exist", ht->root->right->left != NULL);
    mu_assert("Root -> Right -> Left is not the NYT", isNYT(ht->root->right->left));
    mu_assert("Root -> Right -> Left doesn't have NN 507", ht->root->right->left->node_number == 507);
    mu_assert("Root -> Right -> Right doesn't exist", ht->root->right->right != NULL);
    mu_assert("Root -> Right -> Right doesn't have weight 1", ht->root->right->right->weight == 1);
    mu_assert("Root -> Right -> Right isn't a 'B'", ht->root->right->right->element == 'B');
    mu_assert("Root -> Right -> Right doesn't have NN 508", ht->root->right->right->node_number == 508);

    printHuffmanTree(ht);
    printf("Step 3\n");



    printHuffmanTree(ht);
    add_new_element(ht, 'C');
    printf("Printing tree\n");
    printHuffmanTree(ht);

    return 0;
}

static char * all_tests(){
	mu_run_test(test_foo);
	//mu_run_test(test_add_weight_to_element);
	mu_run_test(test_create_huffman_tree);
    mu_run_test(test_create_ht_array);
    mu_run_test(test_last_of_weight);
    mu_run_test(test_huffman_coding);

	return 0;
}
#endif

int main(int argc, char *argv[]){

	#if TEST == 1
		char *result = all_tests();
		if (result != 0) {
			printf("%s\n", result);
		}
		else {
			printf("ALL TESTS PASSED\n");
		}
		printf("Tests run: %d\n", tests_run);
		return 0;
	#endif
	
	char debug_buffer[500];

	if(argc == 1){
		usage();
		return 0;
	}

	if(DEBUG){
		int i;
		for(i = 0; i< argc; i++){
			char buffer[500];
			sprintf(buffer, "Argument %d: %s", i, argv[i]);
			debug(buffer);
		}
	}

	if(strcmp(argv[1], "-t") == 0){
		// Tree test
		
		// First level
		Node* root = createNode(11, 32, -1, NULL, NULL, NULL);

		// Second Level
		Node* n_9 = createNode(9,11, 'f', NULL, NULL, root);
		Node* n_10 = createNode(10,21, -1, NULL, NULL, root);

		// Third level
		Node* n_7 = createNode(7, 10, -1, NULL, NULL, n_10);
		Node* n_8 = createNode(8, 11, -1, NULL, NULL, n_10);

		// Fourth Level
		Node* n_3 = createNode(3, 5, 'c', NULL, NULL, n_7);
		Node* n_4 = createNode(4, 5, -1, NULL, NULL, n_7);
		Node* n_5 = createNode(5, 5, 'd', NULL, NULL, n_8);
		Node* n_6 = createNode(6, 6, 'e', NULL, NULL, n_8);

		// Fifth Level
		Node* n_1 = createNode(1, 2, 'a', NULL, NULL, n_4);
		Node* n_2 = createNode(2, 3, 'b', NULL, NULL, n_4);

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

	if(strcmp(argv[1],"-c") == 0){
		if(argc != 4){
			usage();
			return 1;
		}

		debug("Compression");

		char* file_output = (char*) malloc(strlen(argv[2]));
		char* file_input = (char*) malloc(strlen(argv[3]));
		strcpy(file_output, argv[2]);
		strcpy(file_input, argv[3]);	

		if(DEBUG){
			sprintf(debug_buffer, "Input: %s", file_input);
			debug(debug_buffer);
			sprintf(debug_buffer, "Output: %s", file_output);
			debug(debug_buffer);
		}

		// Check if file exists
		struct stat stat_result;
		int result = stat(file_input, &stat_result);

		if(result != 0){
			// File doesn't exist!
			char buffer[500];
			if(DEBUG){
				sprintf(buffer,"%s does not exist (E: %d).", file_input, result);
			} else {
				sprintf(buffer,"%s does not exist.", file_input);
			}
			error(buffer);
			return 1;
		}

		result = access(file_output, F_OK);
        if(result == -1){
            if(DEBUG){
                char buffer[500];
                sprintf(buffer,"File %s doesn't exist. This is normal. (E%d)", file_output, result);
                warn(buffer);
            }
        }

        result = access(file_output, W_OK);
        if (result != 0) {
            // Cannot write file!
            char buffer[500];
            if (DEBUG) {
                sprintf(buffer, "Unable to write %s: permission denied (E: %d).", file_output, result);
            } else {
                sprintf(buffer, "Unable to write %s: permission denied.", file_output);
            }
            error(buffer);
            return 1;
        }

		// Input exists, Output can be written

		FILE *fh = fopen(file_input, "rb");

		// We'll eventually switch to a buffer for better performances,
		// for now, we stick to our fgetc function since we're working w/ small files

        // Do the Huffman Coding Thing
        HuffmanTree* ht = createHuffmanTree();

        int i = 0;
        for(;;){
			char c = (char) fgetc(fh);
			if(feof(fh)) break;
			if(ferror(fh)) break;
            if(DEBUG) {
                // DEBUG ONLY!
                if (i == 4) break;
            }

            char buffer[200];
            sprintf(buffer, "Parsing byte %02x", c & 0xff);
            debug(buffer);
            add_new_element(ht, c);
            printHuffmanTreeInfo(ht);
            i++;
        }

		printf("\n");
		fclose(fh);
		
		return 0;
	}
}
