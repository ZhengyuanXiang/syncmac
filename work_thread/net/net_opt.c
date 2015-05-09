#include "net_opt.h"
#include "syncmac.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

int start_server()
{
    int serv_fd;
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(struct sockaddr_in));

    serv_fd = socket(AF_INET, SOCK_STREAM, 0);
}