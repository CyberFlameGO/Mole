#ifndef MOLE_LIBRARY_H
#define MOLE_LIBRARY_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/iotcl.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <stdarg.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define BUF_SIZE 2000
#define CLIENT 0
#define SERVER 1
#define PORT 25566
#define IP_HEDR_LEN 20
#define ETH_HEDR_LEN 14
#define ARP_PKT_LEN 28

int turn_alloc(char *dev, int flags);
int crea(int fd, char *buf, int n);
int c_write(int fd, char *buf, int n);
int n_read(int fd, char *buf, int n);
void handle_debug(char *msg, ...);
void handle_errors(char *msg, ...);
void help(void);

#endif