/////////////////////////////////////////////////////////////////////////////////////////
//BORDEAUX INP ENSEIRB-MATMECA
//DEPARTEMENT TELECOM
//RE216 PROGRAMMATION RESEAUX
//{daniel.negru,joachim.bruneau_-_queyreix,nicolas.herbaut}@ipb.fr
////////////////////////////////////////////////////////////////////////////////////////

#ifndef SERVER_H_SDFGEQHAEHZEGFR
#define SERVER_H_SDFGEQHAEHZEGFR

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdbool.h>



/**
 * display an error with perror then quit
 */
void error(const char* );

/**
 * initialize a new ipv6 socket, controlling for error
 * @returns: the file descriptor corresponding to the new server socket
 */
int do_socket(int family, int type, int protocol);

/**
 * initialize the server address
 */
void init_serv_addr(const char* port, struct sockaddr_in *);

/**
 * bind the file descriptor to the configured server
 */
void do_bind(const int fd, struct sockaddr_in *);

/**
 * accept connections on the supplied file descriptor, filling up the sockaddr structure and the client_fd
 */
void do_accept(const int server_fd, int *client_fd,
		struct sockaddr * client_addr);

/**
 * Connect the provided socket to the supplied address
 */
void do_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

/**
 * read a chuck of max 256 chars and return the count of char actually read
 * @returns the number of char actually read
 */
int do_read(const int client_fd, char buffer[256]);

/**
 * write a string to this socket
 * @returns the number of char actually written
 */
int do_write(const int client_socket_fd, const char buffer[256]);




#endif //SERVER_H_SDFGEQHAEHZEGFR
