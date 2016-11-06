#include "channel.h"


// List_Channels list_channels_init(){
// 	List_Channels * list_channels = malloc(sizeof(*list_channels));
// 	list_channels->channel = NULL;
// 	list_channels->next = NULL;

// 	return list_channels;
// }

// Subscribers subscribers_init(){
// 	Subscribers * list_subscribers = malloc(sizeof(*subscribers));
// 	list_subscribers->subscriber = NULL;
// 	list_subscribers->next = NULL;

// 	return list_subscribers;
// }

// void list_channels_add(List_Channels * list_channels, Channel * channel){
// 	// cas ou la liste est vide
// 	if (list_channels->channel == NULL){
// 		list_channels->channel = channel;
// 		return; //on sort de la fonction
// 	}

// 	//si elle n'est pas vide
// 	List_Channels * list_tmp = list_channels;
	
// 	while (list_tmp->next != NULL && list_tmp->channel != NULL){
// 		list_tmp = list_tmp->next;
// 	}

// 	if (list_tmp->next == NULL){ //si on est en bout de chaine
// 		List_Channels list_next = list_channels_init();

// 		list_next->channel = channel;
// 		list_tmp->next = list_next;

// 		return; //sort
// 	}

// 	if (list_next->channel == NULL){ //si on a un trou dans la liste (car next n'est pas null, verifier avant), je vois pas pk mais on sait jamais
// 		list_tmp->channel = channel;
// 		return;
// 	}
// }

// void subscriber_add(Subscribers * list_subscribers, Client * subscriber){
// 	// cas ou la liste est vide
// 	if (list_subscribers->subscriber == NULL){
// 		list_channels->subscriber = subscriber;
// 		return; //on sort de la fonction
// 	}

// 	//si elle n'est pas vide
// 	Subscribers * list_tmp = list_subscribers;
	
// 	while (list_tmp->next != NULL && list_tmp->subscriber != NULL){
// 		list_tmp = list_tmp->next;
// 	}

// 	if (list_tmp->next == NULL){ //si on est en bout de chaine
// 		Subscribers list_next = subscribers_init();

// 		list_next->subscriber = subscriber;
// 		list_tmp->next = list_next;

// 		return; //sort
// 	}

// 	if (list_next->subscriber == NULL){ //si on a un trou dans la liste (car next n'est pas null, verifier avant), je vois pas pk mais on sait jamais
// 		list_tmp->subscriber = subscriber;
// 		return;
// 	}
// }

Channel * channel_create(char * name_channel){
	Channel * channel = malloc(sizeof(*channel));
	strcpy(channel->name, name_channel);

	// Subscribers list_subscribers = subscribers_init();	
	// channel->subscribers = list_subscribers;

	// list_channels_add(channel);
	return channel;
}

// void channel_add_subscriber(List_Channels * list_channels, char * name_channel, Client * client){
// 	List_Channels list_tmp = list_channels;
// 	while ( list_tmp->next != NULL && (strcmp((list_tmp->channel).name, name_channel) == 0) ){
// 		list_tmp = list_tmp->next;
// 	}

// 	Subscribers * list_subs_tmp = (list_tmp->channel).list_subscribers;
// 	subscriber_add(list_subs_tmp, client);
// }

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



// int commande_create(char ** copy_buffer, Client * liste_clients, int i, char * buffer,int compteur){
//     char * argument;
//     int k;

//     argument = strsep(copy_buffer, " ");
//     printf("coucou\n");
//     liste_clients[i].user_channel = (char *)malloc(strlen(argument) * sizeof(char));
//     printf("coucou\n");

//     for (k = 0; k <=compteur; k++) {
//         if(strcmp(argument,liste_clients[k].user_channel) == 0) {
//             snprintf(buffer, BUFFER_SIZE, "Le salon à déja été crée veuillez en utiliser un autre !\n");
//             return 0;
//         }
//     }

//     strcpy(liste_clients[i].user_channel, argument);
//     memset(buffer, 0, BUFFER_SIZE);
//     snprintf(buffer, BUFFER_SIZE, "Vous avez crée le salon: %s\n", liste_clients[i].user_channel);
//     return 1; //on a bien fait la commande create

// }