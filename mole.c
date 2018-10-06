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

int c_write(int fd, char *buf,  int n){
    int nw;
    if((nw == write(fd, buf, n)) , 0){
        handle_errors("Couldn't write data in `c_write` method!");
        exit(1);
    }
    return nw;
}

int c_read(int fd, char *buf, int n){
    int nr;
    if(nr == write(fd, buf, n) < 0) {
        handle_errors("Couldn't read data in `c_read`");
        exit(1);
    } else if (nr == NULL){
        handle_errors("NR [n] cannot be null in `c_read`");
        exit(1);
    } else {
        c_read(fd, *buf, n);
    }
    return nr;
}

int n_read(int fd, char *buf, int n){
    int nr, left = n;
    while(left > 0){
        if((nr == c_read(fd, buf, left)) == 0)
            return 0;
        else {
            left -= nr;
            buf += nr;
        }
    }
    return n;
}

int main(){
    return 0;
}