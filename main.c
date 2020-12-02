#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <strings.h>
#include <sys/uio.h>
#include <sys/stat.h>

#include "lst/list.h"
#include "fs/operations.h"
#include "fh/fileHandling.h"
#include "thr/threads.h"
#include "er/error.h"

//server constants and variables
#define INDIM 30
#define OUTDIM 512
#define TRUE 1

char nameServer[108];
int sockfd;
struct sockaddr_un server_addr;
socklen_t addrlen;
char *path;

int numberThreads = 0;

int modifyingThreads = 0;
int quiescenteThreads = 0;
pthread_cond_t waitQuiescente = PTHREAD_COND_INITIALIZER;
pthread_cond_t waitModifying = PTHREAD_COND_INITIALIZER;

char commandSuccess[10]="SUCCESS";
char commandFail[10]="FAIL";

void startingModifyingCommand(){
    lockMutex();
    while(quiescenteThreads != 0)
        wait(&waitModifying);
    modifyingThreads++;
    unlockMutex();
}

void finishingModifyingCommand(){
    lockMutex();
    modifyingThreads--;
    signal(&waitQuiescente);
    unlockMutex();
}

void startQuiescenteCommand(){
    lockMutex();
    quiescenteThreads++;
    while(modifyingThreads != 0)
        wait(&waitQuiescente);

    unlockMutex();

}

void finishingQuiescenteCommand(){
    lockMutex();
    quiescenteThreads--;
    broadcast(&waitModifying);
    unlockMutex();
}

void applyCommands(list* List){
    
    while(TRUE){

            char token;
            char typeAndName[MAX_FILE_NAME];
            char name[MAX_FILE_NAME]; 
            struct sockaddr_un client_addr;
            char in_buffer[INDIM], out_buffer[OUTDIM];
            int c;

            addrlen=sizeof(struct sockaddr_un);
            c = recvfrom(sockfd, in_buffer, sizeof(in_buffer)-1, 0, (struct sockaddr *)&client_addr, &addrlen);

            if (c <= 0) 
                continue;
            //Preventivo, caso o cliente nao tenha terminado a mensagem em '\0', 
            in_buffer[c]='\0';
            
            printf("Recebeu mensagem de %s\n", client_addr.sun_path);

            int numTokens = sscanf(in_buffer, "%c %s %s", &token, name, typeAndName);
            //free(command);
            if (numTokens < 2)
                errorParse("Error: invalid command in Queue\n");

            int searchResult;

            switch (token) {
                case 'c':
                    switch (typeAndName[0]) {
                        case 'f':
                            startingModifyingCommand();

                            searchResult = create(name, T_FILE, List);

                            finishingModifyingCommand();

                            List = freeItemsList(List, unlockItem);            
                            sendto(sockfd, searchResult, c+1, 0, (struct sockaddr *)&client_addr, addrlen);
                            break;
                        case 'd':
                            startingModifyingCommand();

                            searchResult = create(name, T_DIRECTORY, List);

                            finishingModifyingCommand();

                            List = freeItemsList(List, unlockItem);
                            sendto(sockfd, searchResult, c+1, 0, (struct sockaddr *)&client_addr, addrlen);
                            break;
                        default:
                            sendto(sockfd, FAIL, c+1, 0, (struct sockaddr *)&client_addr, addrlen);
                            errorParse("Error: invalid node type\n");
                    }
                    break;
                case 'l':
                    searchResult = lookup(name, List, 0);
                    List = freeItemsList(List, unlockItem);
                    sendto(sockfd, searchResult, c+1, 0, (struct sockaddr *)&client_addr, addrlen);
                    break;
                case 'd':

                    startingModifyingCommand();

                    searchResult = delete(name, List);

                    finishingModifyingCommand();

                    List = freeItemsList(List, unlockItem);
                    sendto(sockfd, searchResult, c+1, 0, (struct sockaddr *)&client_addr, addrlen);
                    break;

                case 'm':

                    startingModifyingCommand();

                    searchResult = move(name, typeAndName, List);

                    finishingModifyingCommand();
                    List = freeItemsList(List, unlockItem);
                    sendto(sockfd, searchResult, c+1, 0, (struct sockaddr *)&client_addr, addrlen);
                    break;

                case 'p':
                    startQuiescenteCommand();

                    FILE *output = openFile(name, "w");
                    searchResult = SUCCESS;
                    if(output == NULL)
                        searchResult = FAIL;
                    else{
                        print_tecnicofs_tree(output);
                        if(closeFile(output) == NULL)
                            searchResult = FAIL
                    }

                    sendto(sockfd, searchResult, c+1, 0, (struct sockaddr *)&client_addr, addrlen);
                    finishingQuiescenteCommand();
                    break;
                    
                default: { /* error */
                    sendto(sockfd, FAIL, c+1, 0, (struct sockaddr *)&client_addr, addrlen);
                    errorParse("Error: command to apply\n");
                }
            }
    }
}

void *fnThread(void* arg){
    list* inodeList;

    inodeList = createList();

    applyCommands(inodeList);

    /* Free List */
    freeList(inodeList);

    return NULL;
}

int setSockAddrUn(char *path, struct sockaddr_un *addr) {

  if (addr == NULL)
    return 0;

  bzero((char *)addr, sizeof(struct sockaddr_un));
  addr->sun_family = AF_UNIX;
  strcpy(addr->sun_path, path);

  return SUN_LEN(addr);
}

/*  Argv:
        1 -> numThread
        2 -> nameServer */
void setInitialValues(char *argv[]){
    numberThreads = getNumberThreads(argv[1]);
    sprintf(nameServer, "/tmp/%s", argv[2]);
}

int main(int argc, char* argv[]) {
    
    /* Define Arguments */
    setInitialValues(argv);

    if (numberThreads <= 0)
        /* Error Handling */
        errorParse("Error: Wrong number of threads");

    //initializes server
    if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
        perror("server: can't open socket");
        exit(EXIT_FAILURE);
    }

    path = nameServer;

    unlink(path);

    addrlen = setSockAddrUn (nameServer, &server_addr);
    if (bind(sockfd, (struct sockaddr *) &server_addr, addrlen) < 0) {
        perror("server: bind error");
        exit(EXIT_FAILURE);
    }
    
    if (chmod(nameServer, 00222) == -1){
        perror("server:: can't change permission of socket\n");
    }
    
    /* init filesystem */
    init_fs();

    /*creates pool of threads and process input and print tree */
    poolThreads(numberThreads, fnThread);

    /* release allocated memory */
    destroy_fs();
    exit(EXIT_SUCCESS);
}
