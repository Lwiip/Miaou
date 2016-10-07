#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>

#include "config.h"

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

    //gere l'erreur lors de la creation
    if ( option == -1)
        error("ERROR setting socket options");

    return sockfd;
}

void init_serv_addr(struct sockaddr_in * serv_addr) {

    //clean the serv_add structure
    memset(serv_addr, 0, sizeof(serv_addr));

    //cast the port from a string to an int
    //portno = atoi(port); osef car on a mis dans une constante ici coté serveur c'est modifié dans le jalon 2 mais ici osef

    //internet family protocol
    serv_addr->sin_family = AF_INET;

    //we bind to any ip form the host
    serv_addr->sin_addr.s_addr = htonl(INADDR_ANY);

    //we bind on the tcp port specified
    serv_addr->sin_port = htons(PORT);

}

void do_bind(int sock, struct sockaddr_in adr){
    int retour = bind(sock, (struct sockaddr *) &adr, sizeof(adr));

    if( retour == -1 ){
        printf("%i\n",errno );
        perror("erreur lors du bind");
    }
}

void do_listen(int sock){
    //ecoute le message du client (en mode bloquant ici)
    if( listen( sock, SOMAXCONN) == -1 ){
        perror("erreur lors de l'ecoute");
        exit(EXIT_FAILURE);
    }
}

int do_accept(int sock, struct sockaddr_in * adr){
    //creer la sock et les variables necessaires
    int addrlen=sizeof(adr);
    int new_sock=accept(sock, (struct sockaddr *) &adr,&addrlen);

    //gere l'erreur lors de la creation, sinon affiche un message de validation
    if(new_sock==-1)
      printf("Desole, je ne peux pas accepter la session TCP\n");
      else
      printf("connection      : OK\n");
    return new_sock;

}

void do_read(int sockfd, char* buffer){
    memset(buffer, 0, strlen(buffer)); //on s'assure d'avoir des valeurs nulles dans le buff
    int length_r_buff = recv(sockfd, buffer, strlen(buffer) -1, 0); //rempli le buffer
    //si erreur affiche l'erreur, sinon affiche le message envoyé par le client DANS le terminal du serveur (pour check)
    if (length_r_buff < 0) {
        printf("erreur rien n'a été recu\n");
    } else {
        buffer[length_r_buff] = '\0';
        printf("client : ");
        fputs(buffer, stdout);
    }
}

void do_write(int sockfd, char* text){
    //renvoie au client et gere l'enventuelle erreur
    while(send(sockfd, text, strlen(text), 0) == -1){
        printf("erreur envoie\n");
    }
}

//######################
//######################    MAIN
//######################

int main(int argc, char** argv)
{
    //comme notre port est dans le config.h on a pas besoin de passer le port en argument
    // if (argc != 2)
    // {
    //     fprintf(stderr, "usage: RE216_SERVER port\n");
    //     return 1;
    // }

    /*INITIALISATION*/
    struct sockaddr_in serv_addr;
    int lst_sock = do_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


    /*CODE*/
    //init the serv_add structure
    init_serv_addr(&serv_addr);

    //perform the binding
    //we bind on the tcp port specified
    do_bind(lst_sock, serv_addr);

    //specify the socket to be a server socket and listen for at most 20 concurrent client
    do_listen(lst_sock);



    for (;;)
    {
        //accept connection from client
        int rep_sock=do_accept(lst_sock,&serv_addr);

        //read what the client has to say
        char buffer[2048];
        do_read(rep_sock, buffer);

        //we write back to the client
        do_write(rep_sock, buffer);

        //clean up client socket
        int ret = close(rep_sock);
        if (ret == -1) {
            printf("erreur lors de la fermeture de rep_sock\n");
        }
    }

    //clean up server socket
    int ret = close(lst_sock);
    if (ret == -1) {
        printf("erreur lors de la fermeture de lst_sock\n");
    }

    return 0;
}
