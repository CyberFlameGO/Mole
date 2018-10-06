#include "mole.h"
#include <stdio.h>
#include <stdbool.h>
#include <winsock.h>
#include <ws2tcpip.h>

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
    id sin_addr;
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

int tun_alloc(char *dev, int flags){

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

int handle_option(int option, char if_name, int cliserv, char remote_ip[16], unsigned short int port){
    while((option = getopt(_argc, _argv, "i:sc:p:uahd")) > 0){
        switch(option){
            case 'd':
                debug = 1;
                break;
            case 'h':
                //todo print help
                break;
            case 'i':
                strcpy(if_name, IFH_WADDR_LEN - 1);
                break;
            case 's':
                cliserv = SERVER;
                strncpy(remote_ip, optarg, 15);
                break;
            case 'p':
                port = atoi(optarg);
                break;
            default:
                handle_errors("Unkown option %c\n:", option);
                //todo print usage

        }
    }
}

int main(int argc, char *argv[]){

    //basic data sets for the switch above
    int tap_fd, option = 0, flags, maxfd;
    char if_name[IFH_WADDR_LEN] = "";
    int header_len = IP_HEDR_LEN;
    uint16_t nread, nwrite, plength;
    char buffer[BUF_SIZE];
    struct i_sockaddr_in local, remote;
    char remote_ip[16] = "";
    unsigned short int port = PORT;
    int sock_fd, net_fd, optval = 1;
    socklen_t remotelen;
    int cliserv = -1;
    unsigned long int tap2net = 0, net2tap = 0;
    //kinda obvious what this is
    handle_option(option, (char) if_name, cliserv, (char *) remote_ip[16], port);

    argv += optind;
    argc -= optind;

    if(argc > 0){
        handle_errors("Too may options selected!\n");
        //todo print help
    }

    if(*if_name == '\0'){
        handle_errors("Interface name is not specified!\n");
        //todo print help
    } else if(cliserv < 0){
        handle_errors("Must specify a client server, or enable server mode!\n");
        //todo print help (again)
    } else if((cliserv == CLIENT) && (*remote_ip == '\0')){
        handle_errors("An address needs to specified!\n");
        //todo print help (i really need to make this method)
    }

    if((tap_fd == tun_alloc(if_name, flags)) < 0){
        handle_errors("Error while connection to tun/tap interface! %s\n", if_name);
        exit(1);
    }
    handle_debug("Connected to %s interfvace \n", if_name);

    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        handle_errors("socket() could not connect");
        exit(1);
    }

    if(cliserv == CLIENT){
        memset(&remote, 0, sizeof(remote));
        remote.sin_family = AF_INET;
        remote.sin_addr.s_addr = htonl(INADDR_ANY);
        remote.sin_port = htons(port);

        if(connect(sock_fd, (struct i_sockaddr *) &remote, sizeof(remote)) < 0){
            handle_errors("couldn't connect!");
            exit(1);
        }
        net_fd = sock_fd;
        handle_debug("CLIENT connected to server!");
    } else {
        if(setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &optval, sizeof(optval)) < 0){
            handle_errors("Could not handle option handling using setsockopt()");
            exit(1);
        }
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = htonl(INADDR_ANY);
        local.sin_port = htons(port);
        if(bind(sock_fd, (struct i_sockaddr*) &local, sizeof(local)) < 0){
            handle_errors("could not bind locals!");
            exit(1);
        }
        if(listen(sock_fd, 5) < 0){
            handle_errors("could not listen() to the sock_fd!");
            exit(1);
        }

        remotelen = sizeof(remote);
        memset(&remote, 0, remotelen);
        if((net_fd = accept(sock_fd, (struct i_sockaddr*)&remote, &remotelen)) < 0){
            handle_errors("could not accept using sock_fd and remote from sockaddr");
            exit(1);
        }
    }

    return 0;
}