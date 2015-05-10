#ifndef _USER_H_
#define _USER_H_

#include <dir.h>
#include <netinet/in.h>
#include <pthread.h>
#include "syncmac.h"

typedef struct tag_term
{
    unsigned short port;
    char rev[2];
    struct in_addr addr;
    struct tag_term *next;
}SYNC_TERM;

typedef struct tag_user
{
    char name[NAME_MAX];
    SYNC_TERM *next_term;
    DIR_NODE *root;
    struct tag_user *next;
}SYNC_USER;

SYNC_USER *new_sync_user(char *name);
SYNC_TERM *new_sync_term(unsigned short port, struct in_addr *addr);
void free_sync_user(SYNC_USER *user);
void free_sync_term(SYNC_TERM *term);

#endif