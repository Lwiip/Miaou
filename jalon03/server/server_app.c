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


// #include "../commons/network.h"
// #include "server.h"

int server_socket_fd;

#include "../commons/config.h"
#include "../client/client.h"

void error(const char *msg)
{
    perror(msg);
    exit(1);
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

int do_read(int sockfd, char* buffer){
    memset(buffer, 0, BUFFER_SIZE); //on s'assure d'avoir des valuers nulles dans le buff
    int length_r_buff = recv(sockfd, buffer, BUFFER_SIZE -1, 0);

    if (length_r_buff < 0) {
        printf("erreur rien n'a été recu\n");
    } else {
        buffer[length_r_buff] = '\0';
        fputs(buffer, stdout);
    }
    return length_r_buff;
}

void do_write(int sockfd, char* text){
    while(send(sockfd, text, strlen(text), 0) == -1){
        printf("erreur envoie\n");
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

int do_commande(char * buffer, int retour_client, Client * liste_clients, int i, int * compteur, fd_set * readfds){
    char * commande = buffer;
    char * argument;
    char * copy_buffer;
    
    copy_buffer = (char *)malloc(BUFFER_SIZE * sizeof(char));
    argument = (char *)malloc(BUFFER_SIZE * sizeof(char));

    strcpy(copy_buffer,buffer);
    copy_buffer[strlen(copy_buffer) - 1] = '\0'; //-1 pour eviter le \n
    commande = strsep(&copy_buffer, " "); //recupere la commande

    if (strcmp(commande, "/q") == 0 || retour_client == 0){ //si deco
        printf("Deconnection du client\n");
        close(liste_clients[i].lst_sock);
        FD_CLR(liste_clients[i].lst_sock, readfds); //enlève le client de l'ecoute
        *compteur = remove_client(liste_clients, i, *compteur); //met notre client a zero
        return 1; //on ne veut pas rentrer en le send

    } else if (strcmp(commande, "/nick") == 0){
        argument = strsep(&copy_buffer, " ");
        liste_clients[i].pseudo = (char *)malloc(strlen(argument) * sizeof(char));
        strcpy(liste_clients[i].pseudo, argument);



        memset(buffer, 0, BUFFER_SIZE);
        *buffer = strcat("Vous avez été enregistré comme : ", liste_clients[i].pseudo);
        return 0; //on rentre dans le send
    }
    free(copy_buffer);
    free(argument);
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
    char buffer[BUFFER_SIZE];

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
			// liste_clients[compteur].name = ; plus tard
            compteur++; //incremente le nb de client que l'on possède
        } else {

            int i = 0;
            for(i = 0; i < compteur; i++){
			/* un client ecrit */
				if(FD_ISSET(liste_clients[i].lst_sock, &readfds)){

					int retour_client = do_read(liste_clients[i].lst_sock, buffer); //ecoute ce que le client envoie

                    if (do_commande(&buffer, retour_client, liste_clients, i, &compteur, &readfds)){
                        do_write(liste_clients[i].lst_sock, buffer); //repond
                    }
                break;
               }
           }
		}
        a++;
    }

    clear_clients(liste_clients, compteur); //on suprime tout si on ferme le serveur
}
