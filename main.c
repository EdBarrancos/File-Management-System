#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>
#include <pthread.h>

#include "cq/circularqueue.h"
#include "lst/list.h"
#include "fs/operations.h"
#include "fh/fileHandling.h"
#include "thr/threads.h"
#include "er/error.h"

int TRUE = 1;
int numberThreads = 0;
queue* Queue;

//char inputCommands[MAX_COMMANDS][MAX_INPUT_SIZE];
int numberCommands = 0;

FILE *inputFile;
FILE *outputFile;

#define DEBUG 0

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

    if(DEBUG)
        printf("inserted Command %s", data);
    
    return 1;        
}

char* removeCommand() {
    lockMutex();
    char *removedCommand;
    
    while(emptyQueue(Queue) && !getFinishedState(Queue)){
        wait(&waitToNotBeEmpty);
    }

    /* FIXME esta parte nao ta bem, mas nao podemos logo returnar o removeQueue que depois de fazermos removeQueue temos
        de dar signal e unlock */
    removedCommand = removeQueue(Queue);

    if(DEBUG)
        printf("Run Command %s", removedCommand);

    numberCommands--;
    signal(&waitToNotBeFull);
    unlockMutex();

    return removedCommand;
}

void *fnThreadProcessInput(void* arg){
    char line[MAX_INPUT_SIZE];

    if(DEBUG)
        printf("ProcessInput\n");

    /* break loop with ^Z or ^D */

    while (fgets(line, sizeof(line)/sizeof(char), inputFile)) {
        char token, type;
        char name[MAX_INPUT_SIZE];

        int numTokens = sscanf(line, "%c %s %c", &token, name, &type);

        /* perform minimal validation */
        if (numTokens < 1) {
            continue;
        }
        switch (token) {
            case 'c':
                if(numTokens != 3)
                    errorParse("Error: command invalid\n");
                if(insertCommand(line))
                    break;
            
            case 'l':
                if(numTokens != 2)
                    errorParse("Error: command invalid\n");
                if(insertCommand(line))
                    break;
            
            case 'd':
                if(numTokens != 2)
                    errorParse("Error: command invalid\n");
                if(insertCommand(line))
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

    if(DEBUG)
        printf("Finished with commands\n");
    return NULL;
}

void applyCommands(list* List){
    if(DEBUG)
        printf("Lets Start Applying Commands\n");
    
    while(TRUE){

        lockReadRW(&lockR);
        
        if (!getFinishedState(Queue) || !emptyQueue(Queue)){
            const char* command = removeCommand();

            unlockRW(&lockR);

            if (command == NULL){
                continue;
            }

            char token, type;
            char name[MAX_INPUT_SIZE];
            int numTokens = sscanf(command, "%c %s %c", &token, name, &type);
            if (numTokens < 2)
                errorParse("Error: invalid command in Queue\n");

            int searchResult;

            printf("command: %s name: %s type: %c\n",command,name,type);

            switch (token) {
                case 'c':
                    switch (type) {
                        case 'f':
                            printf("Create file: %s\n", name);
                            create(name, T_FILE, List);
                            List = freeItemsList(List, unlockItem);

                            if(DEBUG)
                                printf("Created the cool file: %s\n", name);

                            break;
                        case 'd':
                            printf("Create directory: %s\n", name);
                            create(name, T_DIRECTORY, List);
                            List = freeItemsList(List, unlockItem);

                            if(DEBUG)
                                printf("Created the cool directory: %s\n", name);

                            break;
                        default:
                            errorParse("Error: invalid node type\n");
                    }
                    break;
                case 'l': 
                    searchResult = lookup(name, List);
                    if (searchResult >= 0)
                        printf("Search: %s found\n", name);
                    else
                        printf("Search: %s not found\n", name);
                    List = freeItemsList(List, unlockItem);
                    if(DEBUG)
                        printf("Finished a search\n");
                    break;
                case 'd':
                    printf("Delete: %s\n", name);
                    delete(name, List);
                    List = freeItemsList(List, unlockItem);

                    if(DEBUG)
                        printf("Deleted the cool: %s\n", name);
                    break;
                default: { /* error */
                    errorParse("Error: command to apply\n");
                }
            }
        }

        else{
            TRUE = 0;
            unlockRW(&lockR);
        }
    }
}

void *fnThread(void* arg){
    list* inodeList;

    inodeList = createList();

    applyCommands(inodeList);

    if(DEBUG)
        printf("Free List\n");

    /* Free List */
    freeList(inodeList);

    if(DEBUG)
        printf("Completed Thread\n");

    return NULL;
}

/*  Argv:
        1 -> inputfile
        2 -> outputfile
        3 -> numThreads */
void setInitialValues(char *argv[]){
    inputFile = openFile(argv[1], "r");
    outputFile = openFile(argv[2], "w");
    numberThreads = getNumberThreads(argv[3]);
}

int main(int argc, char* argv[]) {

    struct timeval tvinicio;
    struct timeval tvfinal;

    pthread_cond_init(&waitToNotBeEmpty,NULL);
    
    pthread_cond_init(&waitToNotBeFull,NULL);

    initLockRW(&lockR);

    /* Initialize queue */
    Queue = createQueue();

    /* Define Arguments */
    setInitialValues( argv);

    if (numberThreads <= 0)
        /* Error Handling */
        errorParse("Error: Wrong number of threads");

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
    destroyRW(&lockR);
    gettimeofday(&tvfinal,NULL);
    printf("TecnicoFS completed in %.4f seconds.\n", (double)(tvfinal.tv_sec - tvinicio.tv_sec) + ((tvfinal.tv_usec - tvinicio.tv_usec)/1000000.0));
    exit(EXIT_SUCCESS);
}
