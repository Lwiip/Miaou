#ifndef CLIENT_H
#define CLIENT_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>

#include "../commons/basic.h"
#include "../commons/config.h"


typedef struct $
{
	int sender_mode;
	char file[BUFFER_SIZE];
}Transfert_client;

#endif
