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

#include "cq/circularqueue.h"
#include "lst/list.h"
#include "fs/operations.h"
#include "fh/fileHandling.h"
#include "thr/threads.h"
#include "er/error.h"

//server constants
//#define DIRSERVER "/tmp/"
#define INDIM 30
#define OUTDIM 512
char nameServer[108];

int TRUE = 1;
int numberThreads = 0;
queue* Queue;

//char inputCommands[MAX_COMMANDS][MAX_INPUT_SIZE];
int numberCommands = 0;

FILE *inputFile;
FILE *outputFile;

pthread_cond_t waitToNotBeEmpty, waitToNotBeFull;
pthread_rwlock_t  lockR;


int insertCommand(char* data) {
    lockMutex();

    while(fullQueue(Queue)){
        wait(&waitToNotBeFull);
    }

    insertQueue(Queue, data);
    numberCommands++;
    signal(&waitToNotBeEmpty);
        
    unlockMutex();
    return SUCCESS;        
}

char* removeCommand() {
    lockMutex();
    char *removedCommand;
    
    while(emptyQueue(Queue) && !getFinishedState(Queue)){
        wait(&waitToNotBeEmpty);
    }

    removedCommand = removeQueue(Queue);
    numberCommands--;
    signal(&waitToNotBeFull);
    unlockMutex();

    return removedCommand;
}

void *fnThreadProcessInput(void* arg){
    char line[MAX_INPUT_SIZE];

    /* break loop with ^Z or ^D */

    while (fgets(line, sizeof(line)/sizeof(char), inputFile)) {
        char token;
        char typeAndName[MAX_FILE_NAME];
        char name[MAX_FILE_NAME];

        int numTokens = sscanf(line, "%c %s %s", &token, name, typeAndName);

        /* perform minimal validation */
        if (numTokens < 1) {
            continue;
        }
        switch (token) {
            case 'c':
                if(numTokens != 3)
                    errorParse("Error: command invalid\n");
                if(!insertCommand(line))
                    break;
            
            case 'l':
                if(numTokens != 2)
                    errorParse("Error: command invalid\n");
                if(!insertCommand(line))
                    break;
            
            case 'd':
                if(numTokens != 2)
                    errorParse("Error: command invalid\n");
                if(!insertCommand(line))
                    break;

            case 'm':
                if(numTokens != 3)
                    errorParse("Error: command invalid\n");
                if(!insertCommand(line))
                    break;
            
            case '#':
                break;
            
            default: { /* error */
                errorParse("Error: command invalid\n");
            }
        }
    }
    closeFile(inputFile);
    switchFinishedState(Queue);
    broadcast(&waitToNotBeEmpty);
    return NULL;
}

void applyCommands(list* List){
    
    while(TRUE){

        lockMutex();
        
        if (!getFinishedState(Queue) || !emptyQueue(Queue)){
            unlockMutex();
            char* command = removeCommand();

            if (command == NULL){
                continue;
            }

            char token;
            char typeAndName[MAX_FILE_NAME];
            char name[MAX_FILE_NAME];
            int numTokens = sscanf(command, "%c %s %s", &token, name, typeAndName);
            free(command);
            if (numTokens < 2)
                errorParse("Error: invalid command in Queue\n");

            int searchResult;

            switch (token) {
                case 'c':
                    switch (typeAndName[0]) {
                        case 'f':
                            printf("Create file: %s\n", name);
                            create(name, T_FILE, List);
                            List = freeItemsList(List, unlockItem);
                            break;
                        case 'd':
                            printf("Create directory: %s\n", name);
                            create(name, T_DIRECTORY, List);
                            List = freeItemsList(List, unlockItem);
                            break;
                        default:
                            errorParse("Error: invalid node type\n");
                    }
                    break;
                case 'l': 
                    searchResult = lookup(name, List, 0);
                    if (searchResult >= 0)
                        printf("Search: %s found\n", name);
                    else
                        printf("Search: %s not found\n", name);
                    List = freeItemsList(List, unlockItem);
                    break;
                case 'd':
                    printf("Delete: %s\n", name);
                    delete(name, List);
                    List = freeItemsList(List, unlockItem);
                    break;

                case 'm':
                    printf("Move: %s to %s\n", name, typeAndName);
                    move(name, typeAndName, List);
                    List = freeItemsList(List, unlockItem);
                    break;
                    
                default: { /* error */
                    errorParse("Error: command to apply\n");
                }
            }
        }
        else{
            TRUE = 0;
            unlockMutex();
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
    printf("%s", nameServer);
}

int main(int argc, char* argv[]) {

    //server variables
    int sockfd;
    struct sockaddr_un server_addr;
    socklen_t addrlen;
    char *path;

    //time variables
    struct timeval tvinicio;
    struct timeval tvfinal;

    pthread_cond_init(&waitToNotBeEmpty,NULL);
    
    pthread_cond_init(&waitToNotBeFull,NULL);

    /* Initialize queue */
    Queue = createQueue();

    /* Define Arguments */
    setInitialValues( argv);

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

    
    while (1) {
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

        c = sprintf(out_buffer, "%s", in_buffer);
        
        sendto(sockfd, out_buffer, c+1, 0, (struct sockaddr *)&client_addr, addrlen);

    }

    /* init synch system */
    initLockMutex();
    
    /* init filesystem */
    init_fs();

    /*starts counting the time*/
    gettimeofday(&tvinicio,NULL);

    /*creates pool of threads and process input and print tree */
    poolThreads(numberThreads, fnThread, fnThreadProcessInput);

    /* Free Queue */
    freeQueue(Queue);

    print_tecnicofs_tree(outputFile);
    
    closeFile(outputFile);

    pthread_cond_destroy(&waitToNotBeEmpty);
    
    pthread_cond_destroy(&waitToNotBeFull);

    /* release allocated memory */
    destroy_fs();
    destroyMutex();
    //destroyRW(&lockR);
    gettimeofday(&tvfinal,NULL);
    printf("TecnicoFS completed in %.4f seconds.\n", (double)(tvfinal.tv_sec - tvinicio.tv_sec) + ((tvfinal.tv_usec - tvinicio.tv_usec)/1000000.0));
    //Fechar e apagar o nome do socket, apesar deste programa nunca chegar a este ponto
    //close(sockfd);
    //unlink(argv[1]);
    exit(EXIT_SUCCESS);
}
