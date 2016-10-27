#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>

#include "../commons/config.h"
#include "../commons/basic.h"



/*
Des structs
*/

typedef struct
{

	int lst_sock;
	char * pseudo;
	int registered; //1 pour enregistre, 0 pour non
	time_t connection_date;
	char ip[INET_ADDRSTRLEN];
	int port;

	char * user_channel; //-1 si dans aucune channel

}Client;

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

typedef struct st_subscribers{
    Client subscriber;
    struct st_subscribers * next;
}Subscribers;

typedef struct{
    char * name;
    Subscribers list_subscribers;
}Channel;


/*
Un peu de fonction quand meme
*/

int get_indice_user(Client * liste_clients, char * client_pseudo, int compteur);

#endif
