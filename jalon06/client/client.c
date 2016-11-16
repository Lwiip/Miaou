#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>

#include <libgen.h> //pour basename


#include "client.h"
#include "commands.h"
// #include "../commons/config.h" pas besoin car cet include est deja dans client.h

struct sockaddr_in6 do_connect(int sock, struct sockaddr_in6 sock_host, char* hostname, int port){
    //reinitialise la memoire
    memset(&sock_host, 0, sizeof(sock_host));

    sock_host.sin6_family = AF_INET6;
    inet_aton(hostname, &sock_host.sin6_addr);
    sock_host.sin6_port = htons(port);

    //check de l'erreur
    if (connect(sock, (struct sockaddr *) &sock_host, sizeof(sock_host)) == -1) {
        perror("erreur connect");
        exit(-1);
    }
    return sock_host;
}

int read_line(char *text){
    //recupere la saisie utilisateur (max de BUFFER_SIZE)

    printf("Votre message : ");
    fgets(text, BUFFER_SIZE, stdin);


    if (strlen(text) > 1) {
        return 1;
    } else {
        return 0;
    }
}

void * serveur_client(char * file_name){
    /*INITIALISATION*/
    struct sockaddr_in6 serv_addr;
    int lst_sock = do_socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);


    /*CODE*/
    //init the serv_add structure
    init_serv_addr(PORT_SERVEUR_CLIENT, &serv_addr);

    //perform the binding
    //we bind on the tcp port specified
    do_bind(lst_sock, serv_addr);

    //specify the socket to be a server socket and listen for at most 20 concurrent client
    do_listen(lst_sock);

    //accept connection from client
    int rep_sock=do_accept(lst_sock,&serv_addr);

    //RECEPTION DU FICHIER ICI
    printf("Reception du fichier...\n");

    char recv_buff[CHUNK_SIZE_RECEP];
    char file[CHUNK_SIZE_RECEP];
    snprintf(file, CHUNK_SIZE_RECEP, "%s%s", SAVE_LOCATION, file_name);

    FILE * fp = fopen(file, "w+");
    if(fp == NULL){
        printf("Le fichier ne peut etre creer\n");
    } else {


        memset(recv_buff, 0, CHUNK_SIZE_RECEP);
        int length_r_buff = 1;
        int success = 0;
        while(success == 0)
        {
            while(length_r_buff){
                length_r_buff = recv(rep_sock, recv_buff, CHUNK_SIZE_RECEP, MSG_DONTWAIT    );
                if(length_r_buff < 0)
                {
                    break;
                } else if(!length_r_buff){
                    break;
                }
                int write_sz = fwrite(recv_buff, sizeof(char), strlen(recv_buff), fp);
                
                bzero(recv_buff, CHUNK_SIZE_RECEP);
            }
            printf("Fichier reçu !\n");
            success = 1;
        }
    }
    fclose(fp);

    //clean up client socket
    int ret = close(rep_sock);
    if (ret == -1) {
        printf("erreur lors de la fermeture de rep_sock\n");
    }

    //clean up server socket
    ret = close(lst_sock);
    if (ret == -1) {
        printf("erreur lors de la fermeture de lst_sock\n");
    }
}

void * envoie_fichier_client(char * file, char * ip){

    sleep(1); //pour s'assurer que le serveur demarre avant le client

    struct sockaddr_in6 sock_host;
    int sock;


    //get the socket
    sock = do_socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);

    //connect to remote socket
    sock_host = do_connect(sock, sock_host, ip, PORT_SERVEUR_CLIENT);

    
    //ENVOIE DU FICHIER ICI
    char send_buff[CHUNK_SIZE_RECEP]; // buffer d'envoie de meme taille que celui de recep
    printf("Envoie du fichier...\n");
    FILE *fp = fopen(file, "r");
    if(fp == NULL)
    {
        printf("Echec de l'ouverture du fichier");
        exit(1);
    }
    memset(send_buff, 0, CHUNK_SIZE_RECEP);
    int length_s_buff;
    int sent;
    int nbytes;

    while((length_s_buff = fread(send_buff, sizeof(char), CHUNK_SIZE_RECEP, fp)) > 0){
        int offset = 0;
        while ((sent = send(sock, send_buff + offset, length_s_buff, 0)) > 0 || (sent == -1 && errno == EINTR) ) {
                if (sent > 0) {
                    offset += sent;
                    nbytes -= sent;
                }
        }
        bzero(send_buff, CHUNK_SIZE_RECEP);
    }
    close(sock);
    printf("Transfert fini\n");
}

void * init_reception(void * file_serialized){
    return serveur_client((char *)file_serialized);
}

void * init_envoie(void * transfert_serialized){
    Transfert_client * transfert = (Transfert_client *)transfert_serialized;
    return envoie_fichier_client(transfert->file, transfert->ip);
}

void do_read(int sock, char * text, Transfert_client * transfert_client){
    void * data = malloc(2 * BUFFER_SIZE);

    int length_r_buff = recv(sock, data, 2 * BUFFER_SIZE, 0);

    TRY{
        // length_r_buff = recv(sock, (Transfert *)&transfert, sizeof(transfert), 0);
        if (length_r_buff == 2 *BUFFER_SIZE){ //l'astuce est la, le serveur renvoie une taille se sizeod(transfert) +BUFFER_SIZE donc si on a BUFFER non lu c'etait une donnée de transfert, car nos str sont lim a BUFFER_SIZE
            if (transfert_client->sender_mode){
                // alors c'est nous qui envoyons
                snprintf(transfert_client->ip, BUFFER_SIZE, "%s", (char *)data);

                pthread_t tid_send;
                pthread_create(&tid_send, NULL, init_envoie, (void *)transfert_client);

            } else {
                // on recois
                char file[BUFFER_SIZE];
                snprintf(file, BUFFER_SIZE, "%s", (char *)data);
                
                pthread_t tid_recv;
                pthread_create(&tid_recv, NULL, init_reception, (void *)file);
                
            }

            free(data);
            transfert_client->sender_mode = FALSE;
            return;
        }
        
    } CATCH {
        printf("on est passé dans le catch (enlever ce print)\n");
    }END_TRY;


    if (length_r_buff != BUFFER_SIZE){
        memset(text, 0, BUFFER_SIZE); //On s'assure que text vaut rien
        // length_r_buff = recv(sock, text, strlen(text) - 1, 0);
        text = (char *)data;

        //gere l'erreur, si pas afficher le message
        if (0 >= length_r_buff) {
            perror("erreur lors de la reception");
            printf("serveur crash\n");
            exit(0);
        } else {
            text[length_r_buff] = '\0';
            printf("%c[2K", 27); // efface la ligne dans la console
            fputs(text, stdout);
        }
    }
    free(data);
}

int check_commande_arg(char * buffer, char * commande){
    if ((strlen(buffer) -1)==strlen(commande)) {
        return TRUE;
    }
    return FALSE;
}

int client_commande(char * text, char * user_name, int * registered, char * user_channel, Transfert_client * transfert_client){
    char local_copy_text[BUFFER_SIZE];
    char * copy_text = local_copy_text;
    strcpy(copy_text, text);

    char * commande = strsep(&copy_text, " ");

    if (copy_text != NULL) { //si le texte envoyé ne contenait pas de commande (cas dans un salon)
        copy_text[strlen(copy_text) - 1] = '\0';
    } else {
        commande[strlen(commande) - 1] = '\0';
    }

    
    if (strcmp(commande, COMMAND_NICK) == 0) {
        if ((strlen(text) -1) != strlen(commande)) {
            snprintf(user_name, BUFFER_SIZE, "%s", copy_text);
            *registered = 1;
        }
    }

    if (strcmp(commande, COMMAND_SEND) == 0){
        if (check_commande_arg(text, commande)) {  // Pour éviter le core dump si on a rien mis après la commande
            printf( "Entrez la commande %s [pseudo] \"File/2/send.txt\"\n", COMMAND_SEND);
            return FALSE; //on ne rentre pas dans le send
        }


        char * user_dest_name = strsep(&copy_text, " ");

        if (copy_text == NULL){
            printf("Entrez la commande %s [pseudo] \"File/2/send.txt\"\n", COMMAND_SEND);
            return FALSE;
        }

        char * file_location = strsep(&copy_text, " ");
        char out[BUFFER_SIZE];
        replace_str(file_location, "\"", "", out);

        if (0 != access(out, 0)){  // si le fichier n'existe pas
            printf(TEXT_COLOR_RED "Le fichier est introuvable\n" TEXT_COLOR_RESET);
            return FALSE; //on ne veux pas rentrer dans le send
        }
        transfert_client->sender_mode = TRUE;
        strcpy(transfert_client->file, out);
    }

    if (*registered){ // on a droit qu'a /nick si on est pas enregistré
        if ((strcmp(commande, COMMAND_JOIN) == 0) && strlen(user_channel)==0 ) {
            if ((strlen(text) -1) != strlen(commande)) {
                snprintf(user_channel, BUFFER_SIZE, TEXT_COLOR_GREEN " [ %s ]" TEXT_COLOR_RESET, copy_text);
            }
        }
        if (strcmp(commande, COMMAND_QUIT_CHANNEL) == 0) {
            snprintf(user_channel, BUFFER_SIZE, "");
        }
    }
    return TRUE;
}

int main(int argc,char** argv)
{
    //gere le cas ou aucun argument n'est passé (ou pas assez)
    if (argc != 3)
    {
        fprintf(stderr,"usage: RE216_CLIENT hostname port\n");
        return 1;
    }

    struct sockaddr_in6 sock_host;
    int sock;
    fd_set fds;

    //get address info from the server
    char* hostname = argv[1];
    //get port
    int port = atoi(argv[2]);
    // get_addr_info()

    // Variables pour l'utilisateur coté client
    char user_name[BUFFER_SIZE];
    strcpy(user_name, ""); //met le nom vide
    int registered = 0;

    Transfert_client transfert_client;
    transfert_client.sender_mode = FALSE; //on n'envoie pas de fichier par def

    char user_channel[BUFFER_SIZE];
    strcpy(user_channel, ""); //met le nom de channel vide

    //get the socket
    sock = do_socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);

    //connect to remote socket
    sock_host = do_connect(sock, sock_host, hostname, port);


    //get user input
    char *text = malloc (sizeof (*text) * BUFFER_SIZE);

    // nonblock(NB_ENABLE);

    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
    FD_SET(sock, &fds);

    int envoie;

    for(;; ) {
        // FD_ZERO(&fds);
        // FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
        // FD_SET(sock, &fds);

        // select(sock+1, &fds, NULL, NULL, 0);


        if (FD_ISSET(STDIN_FILENO, &fds)) { //si la modification est faite sur stdin
            // read_line(text);
            memset(text, 0, BUFFER_SIZE);
            // fgets(text, BUFFER_SIZE, stdin);
            read(STDIN_FILENO, text, BUFFER_SIZE);

           envoie = client_commande(text, user_name, &registered, user_channel, &transfert_client);

            if (strcmp(text, "/quit\n\0") == 0) {
                while(-1 == send(sock, text, BUFFER_SIZE, 0)) {
                    perror("erreur lors de l'envoie");
                }
                printf("Deconnection\n");
                break;
            }

            // send message to the server
            // handle_client_message(sock, text);
            if (envoie){
                do_write(sock, text);
            }
        }

        

        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
        FD_SET(sock, &fds);
        printf("%s%s > ", user_name, user_channel);
        fflush(stdout); //force le printf

        select(sock+1, &fds, NULL, NULL, 0);

        if (FD_ISSET(sock, &fds)) { //si la modification est faite sur l'ecoute
            do_read(sock, text, &transfert_client);
        }
    }

    free(text);
    close(sock);
    // nonblock(NB_DISABLE);
    return 0;
}
