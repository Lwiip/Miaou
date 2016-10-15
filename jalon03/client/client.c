#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "client.h"
// #include "../commons/config.h" pas besoin car cet include est deja dans client.h



int do_socket(int domain, int type, int protocol) {
    int sockfd;

    //create the socket
    sockfd = socket(domain, type, protocol);

    //check for socket validity
    if( sockfd == -1 ){
        perror("errreur a la creation de la socket");
    }

    return sockfd;
}

struct sockaddr_in do_connect(int sock, struct sockaddr_in sock_host, char* hostname, int port){
    //reinitialise la memoire
    memset(&sock_host, 0, sizeof(sock_host));

    sock_host.sin_family = AF_INET;
    inet_aton(hostname, & sock_host.sin_addr);
    sock_host.sin_port = htons(port);

    //check de l'erreur
    if (connect(sock, (struct sockaddr *) &sock_host, sizeof(sock_host)) == -1){
        perror("erreur connect");
    }
    return sock_host;
}

int read_line(char *text){
    printf("Votre message : ");
    //recupere la saisie utilisateur (max de BUFFER_SIZE)
    fgets(text, BUFFER_SIZE, stdin);

    if (strlen(text) > 1){
        return 1;
    } else {
        return 0;
    }
}

handle_client_message(int sock, char * text){
    //envoie des messages et gere l'erreur
    while(-1 == send(sock, text, BUFFER_SIZE, 0)) {
        perror("erreur lors de l'envoie");
    }
    //ecoute le message
    memset(text, 0, BUFFER_SIZE); //On s'assure que text vaut rien
    int length_r_buff = recv(sock, text, strlen(text) - 1, 0);

    //gere l'erreur, si pas afficher le message
    if (0 >= length_r_buff) {
        perror("erreur lors de la reception");
    } else {
        text[length_r_buff] = '\0';
        fputs(text, stdout);
    }
}

int main(int argc,char** argv)
{
    //gere le cas ou aucun argument n'est passé (ou pas assez)
    if (argc != 3)
    {
        fprintf(stderr,"usage: RE216_CLIENT hostname port\n");
        return 1;
    }

    struct sockaddr_in sock_host;
    int sock;

    //get address info from the server
    char* hostname = argv[1];
    //get port
    int port = atoi(argv[2]);
    // get_addr_info()


    //get the socket
    sock = do_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //connect to remote socket
    sock_host = do_connect(sock, sock_host, hostname, port);


    //get user input
    char *text = malloc (sizeof (*text) * 1024);

	for(;;){
		//lit l'entrée utilisateur
		if (read_line(text)){

            if (strcmp(text, "/q\n\0") == 0){
                while(-1 == send(sock, text, BUFFER_SIZE, 0)) {
                    perror("erreur lors de l'envoie");
                }
                printf("Deconnection\n");
                break;
            }

    		//send message to the server
    		handle_client_message(sock, text);

       }
    }
    // free(text);
    close(sock);
    return 0;
}
