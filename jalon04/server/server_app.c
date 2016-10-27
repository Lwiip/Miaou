/////////////////////////////////////////////////////////////////////////////////////////
//BORDEAUX INP ENSEIRB-MATMECA
//DEPARTEMENT TELECOM
//RE216 PROGRAMMATION RESEAUX
//{daniel.negru,joachim.bruneau_-_queyreix,nicolas.herbaut}@ipb.fr
////////////////////////////////////////////////////////////////////////////////////////

// #include <resolv.h>
#include <sys/select.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <time.h>


#define FALSE 0
#define TRUE 1

// #include "../commons/network.h"
// #include "server.h"

int server_socket_fd;

#include "../commons/config.h"
#include "../client/client.h"

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


void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void init_message(Message * message){
    message->buffer = (char *)malloc(BUFFER_SIZE * sizeof(char));
    message->dest_name = (char *)malloc(BUFFER_SIZE * sizeof(char));
}
void free_message(Message * message){
    free(message->buffer);
    free(message->dest_name);
}

int do_socket(int domain, int type, int protocol) {
    int sockfd;
    int yes = 1;
    //create the socket
    sockfd = socket(domain, type, protocol);

    //check for socket validity
    if( sockfd == -1 ){
        perror("erreur a la creation de la socket");
    }

    // set socket option, to prevent "already in use" issue when rebooting the server right on
    int option = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if ( option == -1)
        error("ERROR setting socket options");

    return sockfd;
}

void init_serv_addr(int port, struct sockaddr_in * serv_addr) {

    //clean the serv_add structure
    memset(serv_addr, 0, sizeof(serv_addr));

    //cast the port from a string to an int
    //portno = atoi(port); osef car on a mis dans une constante ici coté serveur

    //internet family protocol
    serv_addr->sin_family = AF_INET;

    //we bind to any ip form the host
    serv_addr->sin_addr.s_addr = htonl(INADDR_ANY);

    //we bind on the tcp port specified
    serv_addr->sin_port = htons(port);

}

void do_bind(int sock, struct sockaddr_in adr){

    int retour = bind(sock, (struct sockaddr *) &adr, sizeof(adr));
    if( retour == -1 ){
        printf("%i\n",errno );
        perror("erreur lors du bind");
    }
}

void do_listen(int sock){
    if( listen( sock, SOMAXCONN) == -1 ){
        perror("erreur lors de l'ecoute");
        exit(EXIT_FAILURE);
    }
}

int do_accept(int sock, struct sockaddr_in * adr){
    int addrlen=sizeof(adr);
    int new_sock=accept(sock, (struct sockaddr *) &adr,&addrlen);
    if(new_sock==-1)
      printf("Desole, je ne peux pas accepter la session TCP\n");
      else
      printf("connection      : OK\n");
    return new_sock;

}

int do_read(Message * message){
    memset(message->buffer, 0, BUFFER_SIZE); //on s'assure d'avoir des valuers nulles dans le buff
    int length_r_buff = recv((message->sender).lst_sock, message->buffer, BUFFER_SIZE -1, 0);

    if (length_r_buff < 0) {
        printf("erreur rien n'a été recu\n");
    } else {
        message->buffer[length_r_buff] = '\0';
    }
    return length_r_buff;
}

void do_write(int sockfd, char* text){
    while(send(sockfd, text, strlen(text), 0) == -1){
        printf("erreur envoie\n");
    }
}

void do_send(Message message, Client * liste_clients, int compteur){
    int i = 0;
    switch (message.destination){
        case no_one: ;
            do_write((message.sender).lst_sock, message.buffer);
            break;

        case everyone : ;//everyone
            for (i = 0; i < compteur; ++i){
                if ((message.sender).lst_sock != liste_clients[i].lst_sock){ //on n'envoie pas au sender
                    do_write(liste_clients[i].lst_sock, message.buffer);
                }
            }
            break;

        case user : ;
            for (i = 0; i < compteur; ++i){
                if (strcmp(liste_clients[i].pseudo, message.dest_name) == 0){ //on envoie qu'a la dest
                    do_write(liste_clients[i].lst_sock, message.buffer);
                    return; //sort de la fonction
                }
            }
            break;

        default:
            perror("Aucune destination pour ce message");
            break;
    }  
}


void clear_clients(Client *liste_clients, int compteur)
{
   int i = 0;
   for(i = 0; i < compteur; i++)
   {
      close(liste_clients[i].lst_sock);
   }
}

int remove_client(Client * liste_clients, int i, int compteur){
    /*supprimer le i ème client et decroit le compteur de client*/
    int k = 0;
    int w = k;

    for (k; k < compteur -1; k++) { //jusqu'a compteur -1 car on suppr une personne
        if (k == i) {
            w++;
            free(liste_clients[i].pseudo);
        }
        liste_clients[k] = liste_clients[w];
        w++;
    }

    return --compteur;
}

void set_buffer(char * buffer, char * message){
    memset(buffer, 0, BUFFER_SIZE);
    strcat(buffer, message);
    strcat(buffer, "\n");
}

void display_clients_pseudo(Client * liste_clients, int compteur, char * buffer){
    int i = 0;
    memset(buffer, 0, BUFFER_SIZE);
    for (i = 0; i < compteur; i++) {
        strcat(buffer, "\t - ");
        strcat(buffer, liste_clients[i].pseudo);
        strcat(buffer, "\n");
    }

}

void display_time(time_t connection_date, char * date){
    struct tm temps = * localtime(&connection_date);
    strftime(date, SIZE_DATE, "le %x à %X", &temps);
    puts(date);
}

int get_indice_user(Client * liste_clients, char * client_pseudo, int compteur){
    int i = 0;
    for (i = 0; i < compteur; ++i)
    {
        if (strcmp(liste_clients[i].pseudo, client_pseudo) == 0){
            return i;
        }
    }
    return -1; //on ne l'a pas trouvé

}

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

int commande_nick(char * commande, char ** copy_buffer, Client * liste_clients, int i, char * buffer){
    char * argument;

    if (strcmp(commande, "/nick") == 0){
        argument = strsep(copy_buffer, " ");
        liste_clients[i].pseudo = (char *)malloc(strlen(argument) * sizeof(char));
        strcpy(liste_clients[i].pseudo, argument);

        memset(buffer, 0, BUFFER_SIZE);
        snprintf(buffer, BUFFER_SIZE, "Vous avez été enregistré comme : %s\n", liste_clients[i].pseudo);

        liste_clients[i].registered = 1;

        return 1; //on a bien fait la commande nick

    } else {
        set_buffer(buffer, "Veuillez vous enregistrer avec la commande /nick [pseudo]");
        return 0; //on n'a pas fait la commande nick
    }
}

void display_client_info(Client * liste_clients, int compteur, char * buffer, char ** copy_buffer){
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

void display_help(char * buffer){
    strcat(buffer, "Commandes diponibles :\n"
        "\t- /nick [pseudo] \t> ajoute ou modifie votre pseudo\n"
        "\t- /q \t\t\t> ferme le chat\n"
        "\t- /who \t\t\t> affiche les utilisateurs en ligne\n"
        "\t- /whois [pseudo] \t> affiche les informations relatives au joueur\n");
}

void whisp(char ** copy_buffer, Message * message, Client * liste_clients, int compteur, char * sender_name){
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

        snprintf(message->buffer, BUFFER_SIZE, ">>> Whisp from %s : %s\n", sender_name, *copy_buffer); //supprime la commande et l'argument du message a transmettre
    } else {
        message->destination = user;
        strcpy(message->dest_name, (message->sender).pseudo); //on renvoie un message d'erreur

        snprintf(message->buffer, BUFFER_SIZE, "/!\\ utilisateur non trouvé !\n"); //supprime la commande et l'argument du message a transmettre
    }
}

int do_commande(Message * message, int retour_client, Client * liste_clients, int i, int * compteur, fd_set * readfds){
    char * commande = message->buffer;
    char local_copy_buffer[BUFFER_SIZE];
    char * copy_buffer = local_copy_buffer;

    strcpy(copy_buffer,message->buffer);
    copy_buffer[strlen(copy_buffer) - 1] = '\0'; //-1 pour eviter le \n

    commande = strsep(&copy_buffer, " "); //recupere la commande


    if (commande_quit(commande, retour_client, liste_clients, i, compteur, readfds)){ //si deco
        return 0; //on ne veut pas rentrer en le send
    }

    switch (liste_clients[i].registered) {
    case 0 :
        if (commande_nick(commande, &copy_buffer, liste_clients, i, message->buffer)){
            printf("Le client %i a bien été enregistré comme %s\n", liste_clients[i].lst_sock, liste_clients[i].pseudo );
            message->destination = user;
            strcpy(message->dest_name, liste_clients[i].pseudo);
        }

        return 1; //on rentre dans le send
        break;

    case 1 : //si on est enregistre
        if (strcmp(commande, "/who") == 0) {
            display_clients_pseudo(liste_clients, *compteur, message->buffer);
            message->destination = no_one;
        } else if (strcmp(commande, "/whois") == 0){
            display_client_info(liste_clients, *compteur, message->buffer, &copy_buffer);
            message->destination = no_one;
        } else if (strcmp(commande, "/help") == 0){
            memset(message->buffer, 0, BUFFER_SIZE);
            display_help(message->buffer);
            message->destination = no_one;
        } else if (strcmp(commande, "/a") == 0){ //message to all
            snprintf(message->buffer, BUFFER_SIZE, "%s\n", copy_buffer); //supprime la commande du message a transmettre
            message->destination = everyone;
            memset(message->dest_name, 0, BUFFER_SIZE);
        } else if (strcmp(commande, "/w") == 0){
            whisp(&copy_buffer, message, liste_clients, *compteur, liste_clients[i].pseudo);

        } else { //aucune commande
            if ((message->sender).user_channel == NULL){ //si on n'est PAS dans une channel 
                //il faut au moins une commande pour envoyer un message donc on renvoie une erreur
                memset(message->buffer, 0, BUFFER_SIZE);
                snprintf(message->buffer, BUFFER_SIZE, "Votre message ne contenait pas de commande, cela n'est possible "
                "que si vous etes dans une channel, or ce n'est pas le cas.\n");
                display_help(message->buffer);
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

void get_ip_port_client(int rep_sock, Client * liste_clients, int compteur){

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);

    if (getpeername(rep_sock, (struct sockaddr *)&sin, &len) == -1){
        perror("getpeername");
    } else {
        liste_clients[compteur].port = ntohs(sin.sin_port);
    }

    struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&sin;
    struct in_addr ipAddr = pV4Addr->sin_addr;
    inet_ntop( AF_INET, &ipAddr, liste_clients[compteur].ip, INET_ADDRSTRLEN );


}



//######################
//######################    MAIN
//######################

int main(int argc, char** argv){


	/*
	Initialisation variables
	*/
    if (argc != 2){
        fprintf(stderr, "usage: RE216_SERVER port\n");
        return 1;
    }

    int port = atoi(argv[1]);

    int compteur=0;

    Client liste_clients[MAX_CLIENTS];
    Message message;
    init_message(&message);

    struct sockaddr_in serv_addr;

    fd_set readfds;

    int lst_sock = do_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int max = lst_sock;



	/*
	Debut code
	*/

	/*Initialisation du serveur*/

    //init the serv_add structure
    init_serv_addr(port, &serv_addr);

    //perform the binding
    //we bind on the tcp port specified
    do_bind(lst_sock, serv_addr);

	//specify the socket to be a server socket and listen for at most 20 concurrent client
    do_listen(lst_sock);

    int a = 0;
	/*Boucle du serveur*/
    for (;;){
        int i = 0;
        //on vide l'ensemble readfds
        FD_ZERO(&readfds);

        //on ajoute la socket
        FD_SET(lst_sock, &readfds);

        //parcourt de la liste_clients et ajout des sockets d'écoute dans le fd_set
        for(i = 0; i < compteur; i++){
           FD_SET(liste_clients[i].lst_sock, &readfds);
        }

        //utilisation de select
        if(-1 == select(max + 1, &readfds, 0, 0, 0)){
           perror("erreur dans l'appel a select()");
           exit(errno);
        }

        // Si il y a eu un chgt sur la socket principal d'écoute
        if(FD_ISSET(lst_sock, &readfds)){
    		/*ajoute un client a la liste*/
    		int rep_sock = do_accept(lst_sock,&serv_addr); //accept la connexion



    		printf("\nNouveau client : %i\n", rep_sock);

            //Calcul du max
            if (rep_sock>max){
                max=rep_sock;
            } else {
                max=max;
            }

            //ajout de la nouvelle socket client a l'ecoute
            FD_SET(rep_sock, &readfds);


            liste_clients[compteur].lst_sock = rep_sock; //sauvegarde du client
            liste_clients[compteur].registered = 0; //le client n'est pas encore enregistre
            liste_clients[compteur].connection_date = time(NULL);
            liste_clients[compteur].user_channel = NULL;
            get_ip_port_client(rep_sock, liste_clients, compteur);
            
            printf("%s %i\n", liste_clients[compteur].ip, liste_clients[compteur].port);


            compteur++; //incremente le nb de client que l'on possède
        } else {

            int i = 0;
            for(i = 0; i < compteur; i++){
			/* un client ecrit */
				if(FD_ISSET(liste_clients[i].lst_sock, &readfds)){
                    message.destination = no_one; //par defaut on envoie a personne
                    message.sender = liste_clients[i];
					int retour_client = do_read(&message); //ecoute ce que le client envoie
                    commande_quit("", retour_client, liste_clients, i, &compteur, &readfds); //check crash client (ctrl + c)

                    if (strlen(message.buffer) != 0) { //evite le double select et le cas ou l'utilisateur envoie rien

                        printf("Entrée : %s", message.buffer);

                        if (do_commande(&message, retour_client, liste_clients, i, &compteur, &readfds)){
                            printf("Sortie : %s\n", message.buffer);
                            do_send(message, liste_clients, compteur); //repond
                        }
                        break;
                    }
               }
           }
		}
        a++;
    }

    clear_clients(liste_clients, compteur); //on suprime tout si on ferme le serveur
}
