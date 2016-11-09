/////////////////////////////////////////////////////////////////////////////////////////
//BORDEAUX INP ENSEIRB-MATMECA
//DEPARTEMENT TELECOM
//RE216 PROGRAMMATION RESEAUX
//{daniel.negru,joachim.bruneau_-_queyreix,nicolas.herbaut}@ipb.fr
////////////////////////////////////////////////////////////////////////////////////////

// #include <resolv.h>
#include <sys/select.h>

#include <string.h>
#include <unistd.h>
#include <netinet/in.h> //pour gerer l'ip
#include <string.h>
#include <time.h>

#include "commands.h" // ne pas le mettre dans le .h pour eviter une dependance cyclique
// #include "server.h" //commande.h include deja sever.h

void init_message(Message * message){
    message->buffer = (char *)malloc(BUFFER_SIZE * sizeof(char));
    message->dest_name = (char *)malloc(BUFFER_SIZE * sizeof(char));
}
void free_message(Message * message){
    free(message->buffer);
    free(message->dest_name);
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
    if( retour == -1 ) {
        printf("%i\n",errno );
        perror("erreur lors du bind");
    }
}

void do_listen(int sock){
    if( listen( sock, SOMAXCONN) == -1 ) {
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
    memset(message->buffer, 0, BUFFER_SIZE); //on s'assure d'avoir des valeurs nulles dans le buff
    int length_r_buff = recv((message->sender).lst_sock, message->buffer, BUFFER_SIZE -1, 0);

    if (length_r_buff < 0) {
        printf("erreur rien n'a été recu\n");
    } else {
        message->buffer[length_r_buff] = '\0';
    }
    return length_r_buff;
}

void do_send(Message message, Client * liste_clients, int compteur){
    int i = 0;
    switch (message.destination) {
    case no_one:;
        do_write((message.sender).lst_sock, message.buffer);
        break;

    case everyone:;  //everyone
        for (i = 0; i < compteur; ++i) {
            if ((message.sender).lst_sock != liste_clients[i].lst_sock) { //on n'envoie pas au sender
                do_write(liste_clients[i].lst_sock, message.buffer);
            }
        }
        break;

    case user:;
        for (i = 0; i < compteur; ++i) {
            if (strcmp(liste_clients[i].pseudo, message.dest_name) == 0) { //on envoie qu'a la dest
                do_write(liste_clients[i].lst_sock, message.buffer);
                return; //sort de la fonction
            }
        }
        break;

    case channel:;
        Channel * target_channel = channel_find(message.dest_name, liste_clients, compteur);

        for (i = 0; i < compteur; ++i){
            if (liste_clients[i].channel == target_channel){
                do_write(liste_clients[i].lst_sock, message.buffer);
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

/*
a exporter pour commands
*/
int get_indice_user(Client * liste_clients, char * client_pseudo, int compteur){
    int i = 0;
    for (i = 0; i < compteur; ++i)
    {
        if (strcmp(liste_clients[i].pseudo, client_pseudo) == 0) {
            return i;
        }
    }
    return -1; //on ne l'a pas trouvé

}



void get_ip_port_client(int rep_sock, Client * liste_clients, int compteur){

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);

    if (getpeername(rep_sock, (struct sockaddr *)&sin, &len) == -1) {
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
    if (argc != 2) {
        fprintf(stderr, "usage: RE216_SERVER port\n");
        return 1;
    }

    int port = atoi(argv[1]);



    int compteur=0;

    Client liste_clients[MAX_CLIENTS];

    Message message;
    init_message(&message);

    // List_Channels list_channels = list_channels_init();



    struct sockaddr_in serv_addr;

    fd_set readfds;
    fd_set readfds2;

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
    for (;; ) {
        int i = 0;
        //on vide l'ensemble readfds
        FD_ZERO(&readfds);

        //on ajoute la socket
        FD_SET(lst_sock, &readfds);

        //parcourt de la liste_clients et ajout des sockets d'écoute dans le fd_set
        for(i = 0; i < compteur; i++) {
            FD_SET(liste_clients[i].lst_sock, &readfds);
        }

        readfds2=readfds;
        //utilisation de select
        if(-1 == select(max + 1, &readfds2, 0, 0, 0)) {
            perror("erreur dans l'appel a select()");
            exit(errno);
        }

        // Si il y a eu un chgt sur la socket principal d'écoute
        if(FD_ISSET(lst_sock, &readfds2)) {
            /*ajoute un client a la liste*/
            int rep_sock = do_accept(lst_sock,&serv_addr); //accept la connexion



            printf("\nNouveau client : %i\n", rep_sock);

            //Calcul du max
            if (rep_sock>max) {
                max=rep_sock;
            } else {
                max=max;
            }



            //ajout de la nouvelle socket client a l'ecoute
            FD_SET(rep_sock, &readfds2);


            liste_clients[compteur].lst_sock = rep_sock; //sauvegarde du client
            liste_clients[compteur].registered = 0; //le client n'est pas encore enregistre
            liste_clients[compteur].connection_date = time(NULL);
            liste_clients[compteur].channel = NULL; //dans aucune channel par def
            get_ip_port_client(rep_sock, liste_clients, compteur);

            printf("%s %i\n", liste_clients[compteur].ip, liste_clients[compteur].port);


            compteur++; //incremente le nb de client que l'on possède
        } else {

            int i = 0;
            for(i = 0; i < compteur; i++) {
                /* un client ecrit */
                if(FD_ISSET(liste_clients[i].lst_sock, &readfds2)) {

                    message.destination = no_one; //par defaut on envoie a personne
                    message.sender = liste_clients[i];
                    int retour_client = do_read(&message); //ecoute ce que le client envoie
                    commande_quit("", retour_client, liste_clients, i, &compteur, &readfds2); //check crash client (ctrl + c)

                    if (strlen(message.buffer) != 0) { //evite le double select et le cas ou l'utilisateur envoie rien

                    printf("Entrée : %s", message.buffer);

                    if (do_commande(&message, retour_client, liste_clients, i, &compteur, &readfds2)) {
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
