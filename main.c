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

// https://en.wikipedia.org/wiki/ANSI_escape_code#Colors

#define STYLE_COLOR_RED     "\x1b[31m"
#define STYLE_COLOR_GREEN   "\x1b[32m"
#define STYLE_COLOR_YELLOW  "\x1b[33m"
#define STYLE_COLOR_BLUE    "\x1b[34m"
#define STYLE_COLOR_MAGENTA "\x1b[35m"
#define STYLE_COLOR_CYAN    "\x1b[36m"
#define STYLE_COLOR_RESET   "\x1b[0m"

void usage(){
	printf("%sVIZ compressor %sv%s (%s)\n", STYLE_BOLD, STYLE_NO_BOLD, VERSION, GIT_VERSION);
	//printf("%sUsage%s\n", STYLE_UNDERLINE, STYLE_NO_UNDERLINE);
	printf("Compress: \t viz -c input output.viz\n");
	printf("Extract: \t viz -d input.viz output\n");
}

void debug(char* string){
	if(DEBUG){
		printf("%s[D] %s%s",STYLE_COLOR_BLUE, string, STYLE_COLOR_RESET);
	}
}

int main(int argc, char *argv[]){
	char debug_buffer[500];

	if(argc == 1){
		usage();
		return 0;
	}

	if(DEBUG){
		int i;
		for(i = 0; i< argc; i++){
			char buffer[500];
			sprintf(buffer, "Argument %d: %s\n", i, argv[i]);
			debug(buffer);
		}
	}

	if(strcmp(argv[1],"-c") == 0){
		if(argc != 4){
			usage();
			return 1;
		}

		debug("Compression\n");

		char* file_input = (char*) malloc(strlen(argv[2]));
		char* file_output = (char*) malloc(strlen(argv[3]));
		strcpy(file_input, argv[2]);	
		strcpy(file_output, argv[3]);

		if(DEBUG){
			sprintf(debug_buffer, "Input: %s\n", file_input);
			debug(debug_buffer);
			sprintf(debug_buffer, "Output: %s\n", file_output);
			debug(debug_buffer);
		}

	}

	return 0;
}
