#include "tecnicofs-client-api.h"
char commandSuccess[10]="SUCCESS";
char commandFail[10]="FAIL";

int setSockAddrUn(char *path, struct sockaddr_un *addr) {

  if (addr == NULL)
    return 0;

  bzero((char *)addr, sizeof(struct sockaddr_un));
  addr->sun_family = AF_UNIX;
  strcpy(addr->sun_path, path);

  return SUN_LEN(addr);
}

int tfsCreate(char *path, char nodeType) {

  char command[MAX_INPUT_SIZE];

  sprintf(command,"c %s %c", path, nodeType);

  servlen = setSockAddrUn(nameserver, &serv_addr);

  if (sendto(sockfd, command, strlen(command)+1, 0, (struct sockaddr *) &serv_addr, servlen) < 0) {
    perror("client: sendto error");
    return -1;
  } 

  if (recvfrom(sockfd, buffer, sizeof(buffer), 0, 0, 0) < 0) {
    perror("client: recvfrom error");
    return -1;
  } 

  printf("Recebeu resposta do servidor: Create %s\n", buffer);

  if(!strcmp(buffer, commandSuccess)){
    return 0;
  }
  
  return -1;

}

int tfsDelete(char *path) {

  char command[MAX_INPUT_SIZE];

  sprintf(command,"d %s", path);

  servlen = setSockAddrUn(nameserver, &serv_addr);

  if (sendto(sockfd, command, strlen(command)+1, 0, (struct sockaddr *) &serv_addr, servlen) < 0) {
    perror("client: sendto error");
    return -1;
  } 

  if (recvfrom(sockfd, buffer, sizeof(buffer), 0, 0, 0) < 0) {
    perror("client: recvfrom error");
    return -1;
  } 

  printf("Recebeu resposta do servidor: Delete %s\n", buffer);

  if(!strcmp(buffer, commandSuccess)){
    return 0;
  }
  
  return -1;

}

int tfsMove(char *from, char *to) {

  char command[MAX_INPUT_SIZE];

  sprintf(command,"m %s %s", from, to);

  servlen = setSockAddrUn(nameserver, &serv_addr);

  if (sendto(sockfd, command, strlen(command)+1, 0, (struct sockaddr *) &serv_addr, servlen) < 0) {
    perror("client: sendto error");
    return -1;
  } 

  if (recvfrom(sockfd, buffer, sizeof(buffer), 0, 0, 0) < 0) {
    perror("client: recvfrom error");
    return -1;
  } 

  printf("Recebeu resposta do servidor: Move %s\n", buffer);

  if(!strcmp(buffer, commandSuccess)){
    return 0;
  }
  
  return -1;

}

int tfsLookup(char *path) {

  char command[MAX_INPUT_SIZE];

  sprintf(command,"l %s", path);

  servlen = setSockAddrUn(nameserver, &serv_addr);

  if (sendto(sockfd, command, strlen(command)+1, 0, (struct sockaddr *) &serv_addr, servlen) < 0) {
    perror("client: sendto error");
    return -1;
  } 

  if (recvfrom(sockfd, buffer, sizeof(buffer), 0, 0, 0) < 0) {
    perror("client: recvfrom error");
    return -1;
  } 

  printf("Recebeu resposta do servidor: Lookup %s\n", buffer);

  if(!strcmp(buffer, commandSuccess)){
    return 0;
  }
  
  return -1;

}

int tfsMount(char * sockPath) {

  if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0) ) < 0) {
    perror("client: can't open socket");
    return -1;
  }

  unlink(nameclient);
  clilen = setSockAddrUn (nameclient, &client_addr);
  if (bind(sockfd, (struct sockaddr *) &client_addr, clilen) < 0) {
    perror("client: bind error");
    return -1;
  } 

  return 0;
}

int tfsUnmount() {
  close(sockfd);

  unlink(nameclient);

  return 0;
}
