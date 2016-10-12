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
	char * pseudo;
	int registered; //1 pour enregistre, 0 pour non
}Client;

#endif
