#ifndef CONFIG_H_
#define CONFIG_H_

#define PORT 3310
#define ADDRESS 127.0.0.1
#define MAX_CLIENTS 21

#define BUFFER_SIZE 2048
#define SIZE_DATE 128

//les commandes du serveur (custom maggle !)
static const char COMMAND_QUIT[] = 	"/quit";
static const char COMMAND_NICK[] = 	"/nick";
static const char COMMAND_WHO[] = 	"/who";
static const char COMMAND_WHOIS[] = "/whois";
static const char COMMAND_ALL[] = 	"/msgall";
static const char COMMAND_WHISP[] = "/msg";
static const char COMMAND_HELP[] = 	"/help";
static const char COMMAND_QUIT_CHANNEL[]= "/quitchannel";
static const char COMMAND_JOIN[] = 	"/join";

#endif /* CONFIG */
