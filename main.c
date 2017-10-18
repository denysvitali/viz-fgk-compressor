#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "minunit.h"


#define DEBUG 1
#ifndef TEST
	#define TEST 0
#endif
#define VERSION "0.0.1"

#ifndef GIT_VERSION
#define GIT_VERSION "NO GIT VERSION PROVIDED!"
#endif

// https://gist.github.com/s4y/2396238#file-mdtest-c-L4-L8
#define STYLE_BOLD    "\033[1m"
#define STYLE_NO_BOLD "\033[22m"
#define STYLE_UNDERLINE    "\033[4m"
#define STYLE_NO_UNDERLINE "\033[24m"

// https://en.wikipedia.org/wiki/ANSI_escape_code#Colors

#define STYLE_COLOR_RED     "\x1b[31m"
#define STYLE_COLOR_GREEN   "\x1b[32m"
#define STYLE_COLOR_YELLOW  "\x1b[33m"
#define STYLE_COLOR_BLUE    "\x1b[34m"
#define STYLE_COLOR_MAGENTA "\x1b[35m"
#define STYLE_COLOR_CYAN    "\x1b[36m"
#define STYLE_COLOR_RESET   "\x1b[0m"

/* Compressor Magic Number */
#define MAGIC_NUMBER 0x517D3C0

int tests_run = 0;

void usage(){
	printf("%sVIZ compressor %sv%s (%s)\n", STYLE_BOLD, STYLE_NO_BOLD, VERSION, GIT_VERSION);
	//printf("%sUsage%s\n", STYLE_UNDERLINE, STYLE_NO_UNDERLINE);
	printf("Compress: \t viz -c output.viz inputfile\n");
	printf("Extract: \t viz -d input.viz\n");
}

void debug(char* string){
	if(DEBUG){
		printf("%s[D] %s%s\n",STYLE_COLOR_BLUE, string, STYLE_COLOR_RESET);
	}
}	

void error(char* string){
	printf("%s[E] %s%s\n", STYLE_COLOR_RED, string, STYLE_COLOR_RESET);
}

struct Node{
	int node_number;
	int weight;
	char element;
	struct Node* left;
	struct Node* right;
};

int add_weight_to_element(struct Node* node, char c);
struct Node createNode(int node_number, int weight, char element, struct Node* left, struct Node* right);
struct Node* add_new_element(struct Node* node, char c);	

#if TEST == 1

static char * test_foo(){
	mu_assert("Error, DEBUG is not enabled", TEST==1);
	return 0;
}

static char * test_add_weight_to_element(){
	int a_original_weight = 1;

	struct Node root;
	root.element = 1;
	root.weight = 1;
	root.node_number = 3;

	struct Node nullel;
	
	nullel.element = '\0';
	nullel.weight = 0;
	nullel.node_number = 1;
	nullel.left = NULL;
	nullel.right = NULL;
	
	struct Node child;
	child.element = 'a';
	child.weight = a_original_weight;
	child.node_number = 2;
	child.left = NULL;
	child.right = NULL;

	root.left = &nullel;
	root.right = &child;

	add_weight_to_element(&root, 'a');

	mu_assert("Inserted element isn't in the right place", root.right->weight == a_original_weight + 1);
	return 0;
}

static char * test_add_new_element(){
	struct Node root;
	struct Node* pr;
	root = createNode(0, 0, '\0', NULL, NULL);
	pr = add_new_element(&root, 'a');
	root = *pr;
	mu_assert("Element not added (right issue).", root.right->element == 'a' && root.right->weight == 1);
	mu_assert("Left not copied (left issue).", root.left->element == '\0' && root.left->weight == 0);
	mu_assert("Root not changed (actual node issue).", root.weight == 1 && root.node_number == 2);
	return 0;
}

static char * all_tests(){
	mu_run_test(test_foo);
	mu_run_test(test_add_weight_to_element);
	mu_run_test(test_add_new_element);

	return 0;
}
#endif


int add_weight_to_element(struct Node* node, char c){

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

struct Node createNode(int node_number, int weight, char element, struct Node* left, struct Node* right){
	struct Node n;
	n.node_number = node_number;
	n.weight = weight;
	n.element = element;
	n.left = left;
	n.right = right;
	return n;
}

struct Node* add_new_element(struct Node* node, char c){

		if(node->left == NULL && node->right == NULL){
			// Leaf, our node is an element
	
			if(node->element == '\0'){
				struct Node l, r;
				l = *node;
				r = createNode(1, 1, c, NULL, NULL);
				node->weight = r.weight + l.weight;
				node->element = node->weight+'0';
				node->node_number = 2;
				node->right = &r;
				node->left = &l;
				return node;
			}
			return NULL;
		}
	
		struct Node* res;
		if(node->left != NULL){
			res = add_new_element(node->left, c);
			if(res != NULL){
				node->left = res;
				return node;
			}
		}
		else {
			return NULL;
		}
		if(node->right != NULL){
			res = add_new_element(node->right, c);
			if(res != NULL){
				node->right = res;
				return node;
			}
		}
		return NULL;
	}

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
		
		/*struct Node c2 = createNode(1, 2, '\0', NULL, NULL);
		struct Node c1 = createNode(1, 2, '\0', NULL, NULL);
		struct Node nyt = createNode(1, 0, '\0', NULL, NULL);
		struct Node c1 = createNode(1, 2, '\0', NULL, NULL);
		struct Node c1 = createNode(1, 2, '\0', NULL, NULL);*/


		/*struct Node root;
		root.element = 2;
		root.left = NULL;
		return 0;*/
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

		result = access(file_output, W_OK);
		if(result != 0){
			// Cannot write file!
			char buffer[500];
			if(DEBUG){
				sprintf(buffer,"Unable to write %s: permission denied (E: %d).", file_output, result);
			} else {
				sprintf(buffer,"Unable to write %s: permission denied.", file_output);
			}
			error(buffer);
			return 1;
		}

		// Input exists, Output can be written

		FILE *fh = fopen(file_input, "rb");
		
		/*
		struct Node root;

		root.node_number = 0;
		root.weight = 0;
		root.element = '\0';
		root.left = NULL;
		root.right = NULL;
		*/
		
		for(;;){
			char c = fgetc(fh);
			if(feof(fh)) break;
			if(ferror(fh)) break;
			printf("%02x ", c & 0xff);
		}

		printf("\n");
		fclose(fh);
		
		return 0;
	}
}
