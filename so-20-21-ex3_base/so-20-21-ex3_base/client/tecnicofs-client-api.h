#ifndef API_H
#define API_H
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/stat.h>
#include "tecnicofs-api-constants.h"

//name of client's socket
#define CLIENTSOCKET "/tmp/clientTFS"

int sockfd;
socklen_t servlen, clilen;
struct sockaddr_un serv_addr, client_addr;
char buffer[1024];
char nameserver[108];
char nameclient[108];
char commandSuccess[10]="SUCCESS";
char commandFail[10]="FAIL";
int setSockAddrUn(char *path, struct sockaddr_un *addr);
int tfsCreate(char *path, char nodeType);
int tfsDelete(char *path);
int tfsLookup(char *path);
int tfsMove(char *from, char *to);
int tfsMount(char* serverName);
int tfsUnmount();

#endif /* CLIENT_H */
