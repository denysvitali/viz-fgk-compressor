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

void error_test_fail(char* string){
    printf("%s%s❌ TEST ERROR:  %s%s %s\n", STYLE_COLOR_RED, STYLE_BOLD, STYLE_NO_BOLD, string, STYLE_COLOR_RESET);
}

void warn(char* string){
    printf("%s[W] %s%s\n", STYLE_COLOR_YELLOW, string, STYLE_COLOR_RESET);
}

void test_successful(char* string){
    printf("%s%s✓ %s%s%s\n", STYLE_COLOR_GREEN, STYLE_BOLD, string, STYLE_NO_BOLD, STYLE_COLOR_RESET);
}

void test_info(char* string){
    printf("%s%s➡ Testing \"%s\"%s%s\n", STYLE_COLOR_CYAN, STYLE_BOLD, string, STYLE_NO_BOLD, STYLE_COLOR_RESET);
}

void test_info_end(){
    printf("%s%s⬆️ Test completed%s%s\n\n", STYLE_COLOR_GREEN, STYLE_BOLD, STYLE_NO_BOLD, STYLE_COLOR_RESET);
}