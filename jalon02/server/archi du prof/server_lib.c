/////////////////////////////////////////////////////////////////////////////////////////
//BORDEAUX INP ENSEIRB-MATMECA
//DEPARTEMENT TELECOM
//RE216 PROGRAMMATION RESEAUX
//{daniel.negru,joachim.bruneau_-_queyreix,nicolas.herbaut}@ipb.fr
////////////////////////////////////////////////////////////////////////////////////////

#include <asm-generic/socket.h>
#include <regex.h>
#include <resolv.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdbool.h>
#include "user.h"
#include "../commons/network.h"
#include "server.h"
#include "callbacks.h"
#include <stdio.h>

int handle_server_socket_event(const int server_socket_fd) {

}

void handle_client_socket_event(int client_socket_fd) {

}
void send_broadcast_by_fd(const char buffer[256], const int fd) {

}
void send_broadcast_by_user_name(const char buffer[256], const char* uname_src) {

}

void send_unicast(const char buffer[256], const char *uname_dest,
		const char* uname_src) {

}

void send_multicast(const char buffer[256], const char **unames,
		const char* uname_src) {

}


void process_client_request(const char buffer[256], const int client_socket_fd) {

}
