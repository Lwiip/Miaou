/////////////////////////////////////////////////////////////////////////////////////////
//BORDEAUX INP ENSEIRB-MATMECA
//DEPARTEMENT TELECOM
//RE216 PROGRAMMATION RESEAUX
//{daniel.negru,joachim.bruneau_-_queyreix,nicolas.herbaut}@ipb.fr
////////////////////////////////////////////////////////////////////////////////////////

#ifndef USER_H_
#define USER_H_

#include <stddef.h>
#include <netinet/in.h>



//that's the struture you should use to store your users
struct user{
};

typedef struct user* puser;

//you may want to implements those functions to retreive users
puser get_user_by_fd(const int fd);
puser get_user_by_uname(const char name[32]);
void add_user(const struct user auser);
void remove_user_by_fd(const int fd);
void remove_user_by_uname(const char name[32]);

#endif /* USER_H_ */
