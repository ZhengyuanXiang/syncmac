#include "net_opt.h"
#include "syncmac.h"
#include "user.h"
#include "msg.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define BACK_LOG 20

int readn(int fd, void *vptr, int n)
{
    int nleft;
    int nread;
    char *ptr;

    ptr = vptr;
    nleft = n;
    while (nleft > 0)
    {
        if ((nread = read(fd, ptr, nleft)) < 0)
        {
            if (errno == EINTR)
                nread = 0;
            else
            {
                return -1;
                PRINT("readn error %d\n", errno);
            }
        }
        else if (nread == 0)
        {
            break;
        }
        else 
        {

        }
        nleft -= nread;
        ptr += nread;
    }
    return (n-nleft);
}

int writen(int fd, const void *vptr, int n)
{
    int nleft;
    int nwritten;
    const char *ptr;

    ptr = vptr;
    nleft = n;
    while (nleft > 0)
    {
        if ((nwritten = write(fd, ptr, nleft)) <= 0)
        {
            if (nwritten < 0 && errno == EINTR)
                nwritten = 0;
            else
            {
                return -1;
                PRINT("writen error %d\n", errno);
            }
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return n;
}

int con_serv(char *srv_ip, unsigned short srv_port)
{
    int cli_fd;
    int ret;
    struct sockaddr_in cli_sock_in;
    cli_fd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&cli_sock_in, sizeof(struct sockaddr_in));
    cli_sock_in.sin_family = AF_INET;
    cli_sock_in.sin_port = htons(srv_port);
    ret = inet_pton(AF_INET, srv_ip, &cli_sock_in.sin_addr);
    if (ret == -1)
    {
        if (EAFNOSUPPORT == errno)
        {
            PRINT("it is not an ipv4 addr %s\n", srv_ip);
            return ERR;
        }
        PRINT("unknow error inet_pton %d", errno);
        return ERR;
    }
    ret = connect(cli_fd, (struct sockaddr *)&cli_sock_in, sizeof(struct sockaddr_in));
    if (ret == -1)
    {
        if (ECONNREFUSED == errno)
        {
            PRINT("service is not ready\n");
            return ERR;
        }
        PRINT("unknow error connect %d\n", errno);
        return ERR;
    }
    login_cli(cli_fd, LOGIN, "xiang");
    return OK;
}

void login_serv(int logfd)
{
    MSG_LOGIN msg = {0};
    MSG_LOGIN msg_reply = {0};

    int readlen = 0;
    readlen = readn(logfd, &msg, sizeof(MSG_LOGIN));
    if (readlen != sizeof(MSG_LOGIN))
    {
        PRINT("login serv proc err %d %lu\n", readlen, sizeof(MSG_LOGIN));
        close(logfd);
        return;
    }
    msg.name[NAME_MAX - 1] = 0;
    PRINT("name:%s type:%d\n", msg.name, msg.type);
    msg_reply.type = LOGOK;
    if (sizeof(MSG_LOGIN) != writen(logfd, &msg_reply, sizeof(MSG_LOGIN)))
    {
        PRINT("login clinet proc err\n");
        close(logfd);
        return;
    }
    close(logfd);
}

void login_cli(int logfd, char type, char *name)
{
    MSG_LOGIN msg = {0};
    MSG_LOGIN msg_reply = {0};

    msg.type = type;
    strncpy(msg.name, name, NAME_MAX);
    msg.name[NAME_MAX - 1] = 0;
    if (sizeof(MSG_LOGIN) != writen(logfd, &msg, sizeof(MSG_LOGIN)))
    {
        PRINT("login clinet proc err\n");
        close(logfd);
        return;
    }

    if (sizeof(MSG_LOGIN) != readn(logfd, &msg_reply, sizeof(MSG_LOGIN)))
    {
        PRINT("login serv proc err\n");
        close(logfd);
        return;
    }
    if (msg_reply.type != LOGOK)
    {
        PRINT("login failed \n");
        return;
    }
    PRINT("login success\n");
}

int start_server()
{
    int serv_fd;
    int ret;
    int clinet_fd;
    socklen_t cli_sock_len;
    struct sockaddr_in servaddr, cliaddr;
    bzero(&servaddr, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERVER_PORT);

    serv_fd = socket(AF_INET, SOCK_STREAM, 0);
    ret = bind(serv_fd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in));
    if (ret == -1)
    {
        if (EADDRINUSE == errno)
        {
            PRINT("addr inused\n");
            return ERR;
        }
        PRINT("bind failed %d\n", errno);
        return ERR;
    }
    
    listen(serv_fd, BACK_LOG);
    clinet_fd = accept(serv_fd, (struct sockaddr *)&cliaddr, &cli_sock_len);
    if (clinet_fd == -1)
    {
        PRINT("accept error %d\n", errno);
        return ERR;
    }
    PRINT("clinet connected\n");
    login_serv(clinet_fd);
    new_sync_user("test");
    return OK;
}