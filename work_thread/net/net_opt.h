#ifndef _NET_OPT_
#define _NET_OPT_

#define MAXBUFF 256


int readn(int fd, void *vptr, int n);
int writen(int fd, const void *vptr, int n);
int bind_server(char type);
int con_serv(char *srv_ip, unsigned short srv_port);
void login_serv(int logfd);
void login_cli(int logfd, char type, char *name);

#endif