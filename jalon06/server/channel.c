#include "channel.h"




Channel * channel_create(char * name_channel){
	Channel * channel = malloc(sizeof(*channel));
	strcpy(channel->name, name_channel);
	return channel;
}

void channel_destroy(Channel * channel){
	printf("Destruction du salon\n");
	free(channel);
}

void channel_add_subscriber(Channel * channel){
	channel->nb_client++;
}

void channel_rem_subscriber(Channel * channel){
	if (channel->nb_client > 1){
		channel->nb_client--;
	} else {
		channel_destroy(channel);
	}
}


int channel_exist(Client * liste_clients, char * name_channel, int compteur){
	int i = 0;
	for (i = 0; i < compteur; ++i){
		if ((liste_clients[i].channel)->name != NULL){
			if ( strcmp((liste_clients[i].channel)->name, name_channel	) == 0 ){
				return TRUE;
			}
		}
	}
	return FALSE;
}


Channel * channel_find(char * name_channel, Client * liste_clients, int compteur){
	int i = 0;
	for (i = 0; i < compteur; i++){
		if ((liste_clients[i].channel)->name != NULL){
			if ( strcmp((liste_clients[i].channel)->name, name_channel) == 0 ){
				return liste_clients[i].channel;
			}
		}
	}
}
