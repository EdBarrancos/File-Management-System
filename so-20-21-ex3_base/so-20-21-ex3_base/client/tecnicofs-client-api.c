#include "tecnicofs-client-api.h"

int setSockAddrUn(char *path, struct sockaddr_un *addr) {

  if (addr == NULL)
    return 0;

  bzero((char *)addr, sizeof(struct sockaddr_un));
  addr->sun_family = AF_UNIX;
  strcpy(addr->sun_path, path);

  return SUN_LEN(addr);
}

int tfsCreate(char *filename, char nodeType) {

  servlen = setSockAddrUn(nameserver, &serv_addr);

  if (sendto(sockfd, filename, strlen(filename)+1, 0, (struct sockaddr *) &serv_addr, servlen) < 0) {
    perror("client: sendto error");
    return -1;;
  } 

  if (recvfrom(sockfd, buffer, sizeof(buffer), 0, 0, 0) < 0) {
    perror("client: recvfrom error");
    return -1;;
  } 

  printf("Recebeu resposta do servidor: Create %s\n", buffer);

  return 0;

  
}

int tfsDelete(char *path) {
  return -1;
}

int tfsMove(char *from, char *to) {
  return -1;
}

int tfsLookup(char *path) {
  return -1;
}

int tfsMount(char * sockPath) {

  if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0) ) < 0) {
    perror("client: can't open socket");
    return -1;;
  }

  unlink(CLIENTSOCKET);
  clilen = setSockAddrUn (CLIENTSOCKET, &client_addr);
  if (bind(sockfd, (struct sockaddr *) &client_addr, clilen) < 0) {
    perror("client: bind error");
    return -1;;
  } 
  //talvez mudar e alocar a memoria necessaria apenas 
  strcpy(nameserver,sockPath);

  return 0;
}

int tfsUnmount() {
  return -1;
}
