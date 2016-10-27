#ifndef BASIC_H
#define BASIC_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>


//un peu de booléens
#define FALSE 0
#define TRUE 1

void do_write(int sockfd, char* text);

int do_socket(int domain, int type, int protocol);

#endif
