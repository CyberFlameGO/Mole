#include "mole.h"
#include <stdio.h>
#include <stdbool.h>
#define MOLE_LIBRARY_H

bool debug = true;

void handle_debug(char *msg, ...){
    va_list args;
    if(debug){
        va_start(args, msg);
        vprintf(msg, args);
        va_end(args);
    }
}

void handle_errors(char *msg, ...){
    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);
}

int main(){
    return 0;
}