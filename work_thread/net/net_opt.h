#ifndef _NET_OPT_
#define _NET_OPT_

#define SERVER_PORT 6666
#define MAXBUFF 256

int readn(int fd, void *vptr, int n);
int writen(int fd, const void *vptr, int n);
int start_server();
int con_serv(char *srv_ip, unsigned short srv_port);

#endif