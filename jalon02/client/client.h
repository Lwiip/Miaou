#ifndef CLIENT_H
#define CLIENT_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "../commons/config.h"

typedef struct
{
   
	int lst_sock;
	char name[BUFFER_SIZE]; //pour plus tard
}Client;

#endif
