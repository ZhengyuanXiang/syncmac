#include "user.h"
#include "syncmac.h"
#include <dir.h>
#include <stdlib.h>
#include <stdio.h>

SYNC_USER *new_sync_user(char *name)
{
    SYNC_USER *user = malloc(sizeof(SYNC_USER));
    user->next_term = NULL;
    user->root = NULL;
    memcpy(&(user->name), name, strlen(name) + 1);
    return user;
}

SYNC_TERM *new_sync_term(unsigned short port, struct in_addr *addr)
{
    SYNC_TERM *term = malloc(sizeof(SYNC_TERM));
    term->port = port;
    memcpy(&(term->addr), addr, sizeof(struct in_addr));
    return term;
}

void free_sync_user(SYNC_USER * user)
{
    SYNC_TERM *term, *n_term;

    free_dir(user->root);
    term = user->next_term;
    while (term)
    {
        n_term = term->next;
        free(term);
        term = n_term;
    }
    free(user);
}

void free_sync_term(SYNC_TERM *term)
{
    free(term);
}