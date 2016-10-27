#include "basic.h"

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void do_write(int sockfd, char* text){
    while(send(sockfd, text, strlen(text), 0) == -1){
        perror("erreur envoie\n");
    }
}

int do_socket(int domain, int type, int protocol){
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