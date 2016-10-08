#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include "network.h"
#include "client.h"


void get_addr_info(const char* address, const char* port, struct addrinfo** res) {


}

void handle_client_message(const int socket, char buffer[256]) {

}

void handle_server_message(char buffer[256]) {
}

void handle_file_decline(const char* addr, const char* port) {

}
void handle_file_receive(const char *file_name, const char* addr,
		const char* port) {

}

void handle_send_file(const int socket, const char* file_path) {

}
