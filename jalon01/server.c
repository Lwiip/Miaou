#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <config.h>

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
    if( sock == -1 ){
        perror("creation de la socket");
    }

    // set socket option, to prevent "already in use" issue when rebooting the server right on
    option = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if ( option == -1)
        error("ERROR setting socket options");

    return sockfd;
}

void init_serv_addr(struct sockaddr_in * serv_addr) {
    int portno;

    //clean the serv_add structure
    memset(serv_addr, 0, sizeof(serv_addr));

    //cast the port from a string to an int
    //portno = atoi(port); osef car on a mis dans une constante ici coté serveur

    //internet family protocol
    serv_addr->sin_family = AF_INET;

    //we bind to any ip form the host
    serv_addr->sin_addr.s_addr = INADDR_ANY;

    //we bind on the tcp port specified
    serv_addr->sin_port = htons(PORT);

}

void do_bind(int sock, struct sockaddr_in * adr){
    cast_adr = (struct sockaddr *) adr;

    int retour = bind(sock, cast_adr, sizeof(cast_adr))
    if( retour == -1 ){
        perror("erreur lors du bind");
    }
}

void do_listen(int sock){
    if( listen( sock, SOMAXCONN) == -1 ){
        perror("erreur lors de l'ecoute");
        exit(EXIT_FAILURE);
    }
}

void do_accept(){
    
}

//######################
//######################    MAIN
//######################

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: RE216_SERVER port\n");
        return 1;
    }


    struct sockaddr_in serv_addr;

    sock = do_socket(AF_INET, SOCK_STREAM, IPPROTOCO_TCP)

    //init the serv_add structure
    init_serv_addr(PORT, &serv_addr);

    //perform the binding
    //we bind on the tcp port specified
    do_bind(sock, &serv_addr);

    //specify the socket to be a server socket and listen for at most 20 concurrent client
    do_listen(sock);



    for (;;)
    {

        //accept connection from client
        //do_accept()

        //read what the client has to say
        //do_read()

        //we write back to the client
        //do_write()

        //clean up client socket
    }

    //clean up server socket

    return 0;
}
