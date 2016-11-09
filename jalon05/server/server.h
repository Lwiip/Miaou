#ifndef SERVER_H
#define SERVER_H

#include "../commons/config.h"
#include "../commons/basic.h"
#include "channel.h"



/*
Des structs
*/

typedef enum {
    no_one, //so commande or other stuff : send back
    everyone,
    user,
    channel,
}Send_to;

typedef struct{
    Client sender;
    char * buffer;
    Send_to destination;
    char * dest_name;   //pour une channel ou un user
}Message;

/*
Un peu de fonction quand meme
*/

int get_indice_user(Client * liste_clients, char * client_pseudo, int compteur);

#endif
