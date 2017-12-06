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

static char * test_debug(){
    mu_tag("Debug");
	mu_assert("Error, DEBUG is not enabled", TEST==1);
	return 0;
}

static char * test_create_huffman_tree(){
    mu_tag("Huffman Tree Creation");
	HuffmanTree* ht = createHuffmanTree();
    mu_assert("HT has no NYT", ht->nyt != NULL);
    mu_assert("Tree is NULL!", ht->tree != NULL);
    mu_assert("HT doesn't have a root!", ht->root != NULL);
	return 0;
}

static char * test_last_of_weight(){
    mu_tag("Last Of Weight");
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
    mu_tag("Create Huffman Tree Array");
    HuffmanTree* ht = createHuffmanTree();
    mu_assert("HT.TREE[0] is not NYT / Root", ht->tree[0] == ht->nyt && ht->tree[0] == ht->root);
    add_new_element(ht, 'A');
    mu_assert("HT.TREE[0] doesn't exist", ht->tree[0] != NULL);
    mu_assert("HT.TREE[0] doesn't have weight 1", ht->tree[0]->weight == 1);
    mu_assert("HT.TREE[0] doesn't have NN 511", ht->tree[0]->node_number == 511);
    mu_assert("HT.TREE[0] has a parent", ht->tree[0]->parent == NULL);
    mu_assert("HT.TREE[1] doesn't exist", ht->tree[1] != NULL);
    mu_assert("HT.TREE[1] is not the NYT", isNYT(ht->tree[1]));
    mu_assert("HT.TREE[1] doesn't have NN 509", ht->tree[1]->node_number == 509);
    mu_assert("HT.TREE[2] doesn't exist", ht->tree[2] != NULL);
    mu_assert("HT.TREE[2] doesn't have weight 1", ht->tree[2]->weight == 1);
    mu_assert("HT.TREE[2] doesn't have NN 510", ht->tree[2]->node_number == 510);

    add_new_element(ht, 'B');
    printHuffmanTree(ht);

    mu_assert("HT.TREE[0] does not exist", ht->tree[0] != NULL);
    mu_assert("HT.TREE[0] element is not 256", ht->tree[0]->element == -1);
    mu_assert("HT.TREE[0] weight is not 2", ht->tree[0]->weight == 2);
    mu_assert("HT.TREE[0] NN is not 511", ht->tree[0]->node_number == 511);

    //printf("%p, NN: %d", ht->tree[1], ht->tree[1]->node_number);

    mu_assert("HT.TREE[0] doesn't have HT.TREE[1] on its left", ht->tree[0]->left == ht->tree[1]);
    mu_assert("HT.TREE[0] doesn't have HT.TREE[2] on its right", ht->tree[0]->right == ht->tree[2]);

    freeHuffman(ht);

    return 0;
}

static char * test_huffman_coding(){
    mu_tag("Huffman Coding");
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
    printf("%sStep 3%s\n", STYLE_BOLD, STYLE_NO_BOLD);
    add_new_element(ht, 'C');

    printf("Printing tree\n");
    printHuffmanTree(ht);

    mu_assert("Root doesn't have weight 3", ht->root->weight == 3);
    /*mu_assert("Root -> Left doesn't exist", ht->root->left != NULL);
    mu_assert("Root -> Left doesn't have weight 1", ht->root->left->weight == 1);
    mu_assert("Root -> Left isn't the letter 'A'", ht->root->left->element == 'A');
    mu_assert("Root -> Left doesn't have NN 509", ht->root->left->node_number == 509);
    mu_assert("Root -> Right doesn't exist", ht->root->right != NULL);
    mu_assert("Root -> Right doesn't have weight 1", ht->root->right->weight == 1);
    mu_assert("Root -> Right contains a character (-1 needed!)", ht->root->right->element == -1);
    mu_assert("Root -> Right doesn't have NN 510", ht->root->right->node_number == 510);*/

    return 0;
}

static char* test_swap_nodes(){
    mu_tag("Swap Nodes");
    printf("Testing Node Swap\n");

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

    ht->tree[0] = root;
    ht->tree[1] = two_one;
    ht->tree[2] = two_two;
    ht->tree[3] = three_one;
    ht->tree[4] = three_two;
    // 5 = NULL
    // 6 = NULL
    ht->tree[7] = four_one;
    ht->tree[8] = four_two;

    printHuffmanArray(ht);
    printHuffmanTree(ht);


    // Swapping 509 w 510

    Node* first_p = ht->root->left;
    Node first = *first_p;
    Node* second_p = ht->root->right;
    Node second = *second_p;

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

    // TODO: Uncomment tests after ht->tree swap is completed
    /*mu_assert("ht[1] (Root -> Left) does not exist", ht->tree[1] != NULL);
    mu_assert("ht[1] (Root -> Left) is incorrect", ht->tree[1] == second_p);
    mu_assert("ht[2] (Root -> Right) doesn't exist", ht->tree[2] != NULL);
    mu_assert("ht[2] (Root -> Right) is incorrect", ht->tree[2] == first_p);*/

    freeHuffman(ht);

    return 0;
}

static char* test_utility_get_node_position(){
    // Test getNodeLevel && getNodePosition on an empty HT
    mu_tag("Get Node Position")
    HuffmanTree* ht = createHuffmanTree();
    mu_assert("NYT node level is not 0", getNodeLevel(ht->nyt) == 0);
    mu_assert("NYT node is not at position 0", getNodePosition(ht, ht->nyt) == 0);

    // Test getNodeLevel && getNodePosition on a 3 element HT
    add_new_element(ht, 'A');
    // getNodeLevel tests
    mu_assert("A is not at Node Level 1", getNodeLevel(ht->root->right) == 1);
    mu_assert("NYT is not at Node Level 1", getNodeLevel(ht->nyt) == 1);
    // getNodePosition tests
    mu_assert("NYT isn't at position 1 (Root->Left, 0-based index)", getNodePosition(ht, ht->nyt) == 1);
    mu_assert("Root -> Right isn't at position 2 (0-based index)", getNodePosition(ht, ht->root->right) == 2);
    freeHuffman(ht);
    return 0;
}

static char* test_utility_siblings(){
    mu_tag("Utility Siblings");
    HuffmanTree* ht = createHuffmanTree();
    add_new_element(ht, 'A');

    int sibling_size;
    Node** node_siblings = siblings(ht, 1, &sibling_size);
    /*
    int i;
    for(i=0; i<sibling_size; i++){
        Node s_el = *node_siblings[i];
        printf("Sibling Node: %d\n", s_el.node_number);
    }*/
    mu_assert("Sibling Size is not 2", sibling_size == 2);

    mu_assert("Sibling 1 is NULL", node_siblings[0] != NULL);
    mu_assert("Sibling 1 is not NN 509", node_siblings[0]->node_number == 509);
    mu_assert("Sibling 1 is not the NYT", isNYT(node_siblings[0]));

    mu_assert("Sibling 2 is NULL", node_siblings[1] != NULL);
    mu_assert("Sibling 2 is not NN 510", node_siblings[1]->node_number == 510);
    mu_assert("Sibling 2 is not 'A'", node_siblings[1]->element == 'A');

    freeHuffman(ht);

    //mu_assert("", ht->)
    return 0;
}

static char * all_tests(){
	mu_run_test(test_debug);
    mu_run_test(test_create_ht_array);
    /*mu_run_test(test_swap_nodes);
	mu_run_test(test_create_huffman_tree);
	mu_run_test(test_create_ht_array);
    mu_run_test(test_utility_get_node_position);
    mu_run_test(test_utility_siblings);
	//mu_run_test(test_add_weight_to_element);
    mu_run_test(test_last_of_weight);
    mu_run_test(test_huffman_coding);*/

	return 0;
}
#endif

int main(int argc, char *argv[]){

	#if TEST == 1
		char *result = all_tests();
		if (result != 0) {
			//printf("%s\n", result);
            printf("Test failed.\n");
		}
		else {
			test_successful("ALL TESTS PASSED");
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
        freeHuffman(ht);
		fclose(fh);
		
		return 0;
	}
}
