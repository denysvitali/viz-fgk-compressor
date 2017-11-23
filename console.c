//
// Created by dvitali on 23.11.17.
//

#include <stdio.h>
#include "console.h"
#include "colors.h"
#include "defines.h"

void debug(char* string){
    if(DEBUG){
        printf("%s[D] %s%s\n",STYLE_COLOR_BLUE, string, STYLE_COLOR_RESET);
    }
}

void error(char* string){
    printf("%s[E] %s%s\n", STYLE_COLOR_RED, string, STYLE_COLOR_RESET);
}

void warn(char* string){
    printf("%s[W] %s%s\n", STYLE_COLOR_YELLOW, string, STYLE_COLOR_RESET);
}