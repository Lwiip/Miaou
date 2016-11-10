#include "../commons/basic.h"

#include <netinet/in.h>


typedef struct{
    char name[100];
    int nb_client;
}Channel;

typedef struct{
	char file[BUFFER_SIZE];
	int sock_sender;
	int sock_recv;
}Transfert;

typedef struct
{

	int lst_sock;
	char * pseudo;
	int registered; //1 pour enregistre, 0 pour non
	time_t connection_date;
	char ip[INET_ADDRSTRLEN];
	int port;

	int etat_transfert; //-1 en attente de confirmation 0 pas de transfert, 1 envoie 2 recois 3 on ne veut pas	
	Transfert * transfert;

	Channel * channel; //null si dans aucune

}Client;



typedef struct list_channels{
	Channel * channel;
    struct list_channels * next;
}List_Channels;




void list_channels_init(List_Channels * list_channels);

void channel_add_subscriber(Channel * channel);

void channel_rem_subscriber(Channel * channel);

Channel * channel_find(char * name_channel, Client * liste_clients, int compteur);

Channel * channel_create(char * name_channel);

int channel_exist(Client * liste_clients, char * name_channel, int compteur);
