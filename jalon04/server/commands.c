#include <time.h> //juste dans le .c

#include "commands.h"


/*
Utilisée dans server pour gerer les cas de crash client donc a exporter
*/
int commande_quit(char * commande, int retour_client, Client * liste_clients, int i, int * compteur, fd_set * readfds){
    if (strcmp(commande, "/q") == 0 || retour_client == 0){ //si deco
        printf("Deconnection du client\n");
        close(liste_clients[i].lst_sock);
        FD_CLR(liste_clients[i].lst_sock, readfds); //enlève le client de l'ecoute
        *compteur = remove_client(liste_clients, i, *compteur); //met notre client a zero
        return 1; //on a bien fait un /q
    } else {
        return 0; //on n'a pas fait un /q
    }
}

/*
Pas besoin de l'exporter dans un .h car elle est interne a commande
*/
int commande_nick(char * commande, char ** copy_buffer, Client * liste_clients, int i, char * buffer, int change_name){
    char * argument;

    if ((strlen(buffer) -1)==strlen(commande) && !change_name ){  //éviter le seg fault si juste /nick
        snprintf(buffer, BUFFER_SIZE, "Entrez un pseudo valide avec /nick pseudo!\n");
        return 0;
    }

    if (strcmp(commande, "/nick") == 0){
        argument = strsep(copy_buffer, " ");

        if (!change_name) {
            liste_clients[i].pseudo = (char *)malloc(strlen(argument) * sizeof(char));
        }
        strcpy(liste_clients[i].pseudo, argument);

        memset(buffer, 0, BUFFER_SIZE);
        snprintf(buffer, BUFFER_SIZE, "Vous avez été enregistré comme : %s\n", liste_clients[i].pseudo);

        liste_clients[i].registered = 1;

        return 1; //on a bien fait la commande nick

    } else if (!change_name) {
        memset(buffer, 0, BUFFER_SIZE);
        snprintf(buffer, BUFFER_SIZE, "Veuillez vous enregistrer avec la commande /nick [pseudo]");
        return 0; //on n'a pas fait la commande nick
    }
}



/*
Pas d'export
*/
void commande_who(Client * liste_clients, int compteur, Message * message){
    int i = 0;
    memset(message->buffer, 0, BUFFER_SIZE);
    for (i = 0; i < compteur; i++) {
        strcat(message->buffer, "\t - \'");
        strcat(message->buffer, liste_clients[i].pseudo);
        strcat(message->buffer, "\'\n");
    }

    message->destination = no_one;
}

void display_time(time_t connection_date, char * date){
    struct tm temps = * localtime(&connection_date);
    strftime(date, SIZE_DATE, "le %x à %X", &temps);
    puts(date);
}

/*
Pas a exporter
*/
void commande_whois(Client * liste_clients, int compteur, char * buffer, char ** copy_buffer){
    char * argument = strsep(copy_buffer, " ");
    int indice = get_indice_user(liste_clients, argument, compteur);

    if (indice != -1){ //si on a trouvé

        char date[SIZE_DATE];
        display_time(liste_clients[indice].connection_date, date);

        memset(buffer, 0, BUFFER_SIZE);
        snprintf(buffer, BUFFER_SIZE, "%s est connecté depuis %s avec l'ip %s et le port %i\n"
            , argument, date, liste_clients[indice].ip, liste_clients[indice].port);

    } else {
        memset(buffer, 0, BUFFER_SIZE);
        snprintf(buffer, BUFFER_SIZE, "Le client %s est inconnu(e)\n", argument);

    }
}


/*
Pas a exporter
*/
void commande_help(Message * message){
    memset(message->buffer, 0, BUFFER_SIZE);
    strcat(message->buffer, "Commandes diponibles :\n"
        "\t- /q \t\t\t> ferme le chat\n"
        "\t- /who \t\t\t> affiche les utilisateurs en ligne\n"
        "\t- /whois [pseudo] \t> affiche les informations relatives au joueur\n");

    message->destination = no_one;
}

/*
Pas a exporter
*/
void commande_whisp(char ** copy_buffer, Message * message, Client * liste_clients, int compteur, char * sender_name){
    char * argument = strsep(copy_buffer, " ");

    memset(message->buffer, 0, BUFFER_SIZE);

    int i = 0;
    int exist = FALSE;
    for (i = 0; i < compteur; ++i){
        if (strcmp(liste_clients[i].pseudo, argument) == 0){ //si on trouve le destinataire
            exist = TRUE;
            break;
        }
    }

    if (exist){
        message->destination = user;
        strcpy(message->dest_name, argument);

        snprintf(message->buffer, BUFFER_SIZE, "%s[%s] : %s\n%s", TEXT_COLOR_MAGENTA ,sender_name, *copy_buffer, TEXT_COLOR_RESET); //supprime la commande et l'argument du message a transmettre
    } else {
        message->destination = no_one; //on renvoie un message d'erreur

        snprintf(message->buffer, BUFFER_SIZE, "/!\\ utilisateur non trouvé !\n"); //supprime la commande et l'argument du message a transmettre
    }
}



/*
A exporter ça ^^
*/
int do_commande(Message * message, int retour_client, Client * liste_clients, int i, int * compteur, fd_set * readfds){
    char * commande = message->buffer;
    char local_copy_buffer[BUFFER_SIZE];
    char * copy_buffer = local_copy_buffer;

    strcpy(copy_buffer, message->buffer);
    copy_buffer[strlen(copy_buffer) - 1] = '\0'; //-1 pour eviter le \n

    commande = strsep(&copy_buffer, " "); //recupere la commande


    if (commande_quit(commande, retour_client, liste_clients, i, compteur, readfds)){ //si deco
        return 0; //on ne veut pas rentrer en le send
    }

    switch (liste_clients[i].registered) {
    case 0 :
        if (commande_nick(commande, &copy_buffer, liste_clients, i, message->buffer, 0)){
            printf("Le client %i a bien été enregistré comme %s\n", liste_clients[i].lst_sock, liste_clients[i].pseudo );
        }
        message->destination = no_one; //repond au client

        return 1; //on rentre dans le send
        break;

    case 1 : //si on est enregistre
        if (strcmp(commande, "/nick") == 0){
            if (commande_nick(commande, &copy_buffer, liste_clients, i, message->buffer, 1)){
                printf("Le client %i a bien été enregistré comme %s\n", liste_clients[i].lst_sock, liste_clients[i].pseudo );
            }
        } else if (strcmp(commande, "/who") == 0) {
            commande_who(liste_clients, *compteur, message);
        } else if (strcmp(commande, "/whois") == 0){
            commande_whois(liste_clients, *compteur, message->buffer, &copy_buffer);
            message->destination = no_one;
        } else if (strcmp(commande, "/help") == 0){
            commande_help(message);
        } else if (strcmp(commande, "/a") == 0){ //message to all
            snprintf(message->buffer, BUFFER_SIZE, "[%s] : %s\n", (message->sender).pseudo, copy_buffer); //supprime la commande du message a transmettre
            message->destination = everyone;
            memset(message->dest_name, 0, BUFFER_SIZE);
        } else if (strcmp(commande, "/w") == 0){
            commande_whisp(&copy_buffer, message, liste_clients, *compteur, liste_clients[i].pseudo);

        } else { //aucune commande
            if ((message->sender).user_channel == NULL){ //si on n'est PAS dans une channel 
                //il faut au moins une commande pour envoyer un message donc on renvoie une erreur
                memset(message->buffer, 0, BUFFER_SIZE);
                snprintf(message->buffer, BUFFER_SIZE, "Votre message ne contenait pas de commande, cela n'est possible "
                "que si vous etes dans une channel, or ce n'est pas le cas.\n");
                commande_help(message);
            } else {
                message->destination = channel;
                message->dest_name = (message->sender).user_channel;
            }
        }

        return 1;//si aucune commande, peutetre que c'est juste un message donc on rentre dans send
        break;

    default :
        perror("liste_clients.registered different de 0 ou 1 !");
        return 0;
        break;

    }
}