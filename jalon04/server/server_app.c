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


// #include "../commons/network.h"
// #include "server.h"

int server_socket_fd;

#include "../commons/config.h"
#include "../client/client.h"

typedef enum {
    no_one,
    everyone,
    user,
    channel,
}Send_to;

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
    snprintf(buffer, BUFFER_SIZE, "Commandes diponibles :\n"
        "\t- /nick [pseudo] `\t> ajoute ou modifie votre pseudo\n"
        "\t- /q \t\t> ferme le chat\n"
        "\t- /who \t\t> affiche les utilisateurs en ligne\n"
        "\t- /whois [pseudo] \t> affiche les informations relatives au joueur\n");
}

int do_commande(char * buffer, int retour_client, Client * liste_clients, int i, int * compteur, fd_set * readfds){
    char * commande = buffer;
    char local_copy_buffer[BUFFER_SIZE];
    char * copy_buffer = local_copy_buffer;

    strcpy(copy_buffer,buffer);
    copy_buffer[strlen(copy_buffer) - 1] = '\0'; //-1 pour eviter le \n

    commande = strsep(&copy_buffer, " "); //recupere la commande


    if (commande_quit(commande, retour_client, liste_clients, i, compteur, readfds)){ //si deco
        return 0; //on ne veut pas rentrer en le send
    }

    switch (liste_clients[i].registered) {
    case 0 :
        if (commande_nick(commande, &copy_buffer, liste_clients, i, buffer)){
            printf("Le client %i a bien été enregistré comme %s\n", liste_clients[i].lst_sock, liste_clients[i].pseudo );
        }

        return 1; //on rentre dans le send
        break;

    case 1 : //si on est enregistre
        if (strcmp(commande, "/who") == 0) {
            display_clients_pseudo(liste_clients, *compteur, buffer);
        } else if (strcmp(commande, "/whois") == 0){
            display_client_info(liste_clients, *compteur, buffer, &copy_buffer);
        } else if (strcmp(commande, "/help") == 0){
            memset(buffer, 0, BUFFER_SIZE);
            strcat(buffer, "tst\n");
            display_help(buffer);
        } else {

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
            liste_clients[compteur].registered = 0; //le client n'est pas encore enregistre
            liste_clients[compteur].connection_date = time(NULL);
            get_ip_port_client(rep_sock, liste_clients, compteur);
            
            printf("%s %i\n", liste_clients[compteur].ip, liste_clients[compteur].port);


            compteur++; //incremente le nb de client que l'on possède
        } else {

            int i = 0;
            for(i = 0; i < compteur; i++){
			/* un client ecrit */
				if(FD_ISSET(liste_clients[i].lst_sock, &readfds)){

					int retour_client = do_read(liste_clients[i].lst_sock, buffer); //ecoute ce que le client envoie
                    commande_quit("", retour_client, liste_clients, i, &compteur, &readfds); //check crash client (ctrl + c)

                    if (strlen(buffer) != 0) { //evite le double select et le cas ou l'utilisateur envoie rien

                        printf("Entrée : %s", buffer);

                        if (do_commande(buffer, retour_client, liste_clients, i, &compteur, &readfds)){
                            printf("Sortie : %s\n", buffer);
                            do_write(liste_clients[i].lst_sock, buffer); //repond
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
