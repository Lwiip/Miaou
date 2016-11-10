#ifndef BASIC_H
#define BASIC_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> //pour gerer l'ip
#include <string.h>
#include <errno.h>

#include "config.h"

#include <setjmp.h>

// Bon on se creer les try et catch car c'est bien pratique quand meme ^^ 
#define TRY do{ jmp_buf env; if( !setjmp(env) ){
#define CATCH } else {
#define END_TRY } }while (0)
#define THROW longjmp(env, 1)

//un peu de booléens
#define FALSE 0
#define TRUE 1

//Un peu de couleur aussi
#define TEXT_COLOR_RED 		"\x1b[31m"
#define TEXT_COLOR_GREEN 	"\x1b[32m"
#define TEXT_COLOR_YELLOW 	"\x1b[33m"
#define TEXT_COLOR_BLUE 	"\x1b[34m"
#define TEXT_COLOR_MAGENTA 	"\x1b[35;1m"
#define TEXT_COLOR_CYAN 	"\x1b[36m"
#define TEXT_COLOR_BOLD		"\x1b[1m"
#define TEXT_COLOR_UNBOLD	"\x1b[21m"
#define TEXT_COLOR_RESET	"\x1b[0m"



void do_write(int sockfd, char* text);

int do_socket(int domain, int type, int protocol);

void init_serv_addr(int port, struct sockaddr_in * serv_addr);

void do_bind(int sock, struct sockaddr_in adr);

void do_listen(int sock);

int do_accept(int sock, struct sockaddr_in * adr);

void replace_str(char * str, char * origine, char * replacement, char * out);

#endif
