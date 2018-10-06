#include "mole.h"
#include <stdio.h>
#include <stdbool.h>
#include <winsock.h>

#define MOLE_LIBRARY_H
#define O_RWDR
#define TUNSETIFF 0

bool debug = true;

struct i_sockaddr {
   unsigned short sa_family;
   signed char sa_data[14];
};

struct i_sockaddr_in {
    signed short sin_family;
    unsigned short sin_port;
    signed char sin_zero[8];
};

struct freq {
#define IFH_WADDR_LEN 6
    union {
        char ifrn_name[IFH_WADDR_LEN];
    }ifr_ifrn;

    union {
        struct i_sockaddr ifru_addr;
        struct i_sockaddr ifru_dst_addr;
        struct i_sockaddr ifru_borad_addr;
        struct i_sockaddr ifru_network_mask;
        struct i_sockaddr ifru_hw_addr;
        signed short ifru_flags;
        signed int ifru_i_values;
        signed int ifru_mtu;
        signed char ifru_slave[IFH_WADDR_LEN];
        signed char ifru_new_name[IFH_WADDR_LEN];
        void * ifru_data;
    };
};

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

int turn_alloc(char *dev, int flags){

    struct freq ifr;
    int fd, err;

    if((fd = open("/dev/net/tun", 'r')) < 0){
        handle_errors("Could not open /dev/net/tun");
        return fd;
    }
    memset(&ifr, 0, sizeof(ifr));

    ifr.ifru_flags = flags;

    if(*dev){
        strncpy(ifr.ifr_ifrn.ifrn_name, dev, IFH_WADDR_LEN);
    }

    if((err == ioctlsocket(fd, TUNSETIFF, (void *) &ifr)) < 0){
        handle_errors("ioctlsocket could not pass TUNSETIFF");
        close(fd);
        return err;
    }
    strcpy(dev, ifr.ifr_ifrn.ifrn_name);
    return fd;
}

int main(){
    return 0;
}