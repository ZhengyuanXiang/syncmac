#ifndef _MSG_H_
#define _MSG_H_

#include "syncmac.h"

#define LOGIN 1
#define LOGOUT 2
#define LOGOK 3
#define LOGFAIL 4

typedef struct tag_msg_login
{
    char type;
    char recv[7];
    char name[NAME_MAX];
}MSG_LOGIN;

#endif