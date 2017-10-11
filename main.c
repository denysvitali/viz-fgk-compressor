#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>


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

/* Compressor Magic Number */
#define MAGIC_NUMBER 0x517D3C0

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
			sprintf(buffer, "Argument %d: %s", i, argv[i]);
			debug(buffer);
		}
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
		
		char c = fgetc(fh);
		do {
			printf("%02x ", c & 0xff);
			c=fgetc(fh);
		}
		while(!feof(fh));

		print("\n");
		fclose(fh);
		

	}

	return 0;
}
