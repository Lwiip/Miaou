#ifndef COMMANDS_H
#define COMMANDS_H

#include "../commons/basic.h"
#include "server.h"



int commande_quit(char * commande, int retour_client, Client * liste_clients, int i, int * compteur, fd_set * readfds);

int do_commande(Message * message, int retour_client, Client * liste_clients, int i, int * compteur, fd_set * readfds);

#endif
