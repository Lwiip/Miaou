/////////////////////////////////////////////////////////////////////////////////////////
//BORDEAUX INP ENSEIRB-MATMECA
//DEPARTEMENT TELECOM
//RE216 PROGRAMMATION RESEAUX
//{daniel.negru,joachim.bruneau_-_queyreix,nicolas.herbaut}@ipb.fr
////////////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/select.h>
#include <resolv.h>
#include <string.h>
#include <sys/select.h>
#include <stdlib.h>
#include "../commons/network.h"
#include "server.h"
#include "client.h"
#include "../commons/network.h"
#include "../commons/contrib.h"

int main(int argc, char** argv) {

	if (argc != 3) {
		fprintf(stderr, "usage: RE216_CLIENT hostname port\n");
		return 1;
	}

	//get the add info

	//get the socket

	//connect to remote socket

	//in an infinite loop, use select to listen to open sockets
	for (;;) {

	}

}

