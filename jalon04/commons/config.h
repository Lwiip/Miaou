#ifndef CONFIG_H_
#define CONFIG_H_

#define PORT 3310
#define ADDRESS 127.0.0.1
#define MAX_CLIENTS 21

#define BUFFER_SIZE 2048
#define SIZE_DATE 128

//les commandes du serveur (custom maggle !)
static const char COMMAND_QUIT[] = 	"/q";
static const char COMMAND_NICK[] = 	"/n";
static const char COMMAND_WHO[] = 	"/who";
static const char COMMAND_WHOIS[] = "/whois";
static const char COMMAND_ALL[] = 	"/a";
static const char COMMAND_WHISP[] = "/w";
static const char COMMAND_HELP[] = 	"/h";

#endif /* CONFIG */
