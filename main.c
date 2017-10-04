#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define DEBUG 1
#define VERSION "0.0.1"

#ifndef GIT_VERSION
#define GIT_VERSION "NO GIT VERSION PROVIDED!"
#endif

// https://gist.github.com/s4y/2396238#file-mdtest-c-L4-L8
#define STYLE_BOLD    "\033[1m"
#define STYLE_NO_BOLD "\033[22m"
#define STYLE_UNDERLINE    "\033[4m"
#define STYLE_NO_UNDERLINE "\033[24m"

void usage(){
	printf("%sVIZ compressor %sv%s (%s)\n", STYLE_BOLD, STYLE_NO_BOLD, VERSION, GIT_VERSION);
	//printf("%sUsage%s\n", STYLE_UNDERLINE, STYLE_NO_UNDERLINE);
	printf("Compress: \t viz -c input output.viz\n");
	printf("Extract: \t viz -d input.viz output\n");
}

int main(int argc, char *argv[]){
	if(DEBUG){
		//printf("%d arguments\n", argc);
	}

	if(argc == 1){
		usage();
		return 0;
	}

	if(DEBUG){
		int i;
		for(i = 0; i< argc; i++){
			printf("Argument %d: %s\n", i, argv[i]);
		}
	}

	if(strcmp(argv[1],"-c") == 0){
		// Argument max length defined by OS
		// (getconf ARG_MAX results in 2097152 (2M) on my PC, YMMV)

		if(argc != 4){
			usage();
			return 1;
		}

		char* file_input[_POSIX_ARG_MAX];
		char* file_output[_POSIX_ARG_MAX];
		if(DEBUG) { 
			printf("Compression\n");
		}
		strcpy(file_input, argv[2]);	
		strcpy(file_output, argv[3]);	

		printf("Input: %s\n", file_input);
		printf("Output: %s", file_output);

	}

	return 0;
}
