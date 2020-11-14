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

int numberThreads = 0;
queue* Queue;

//char inputCommands[MAX_COMMANDS][MAX_INPUT_SIZE];
int numberCommands = 0;

FILE *inputFile;
FILE *outputFile;

#define DEBUG 1

pthread_cond_t waitToNotBeEmpty, waitToNotBeFull;


int insertCommand(char* data) {
    /* FIXME -> use new functions */
    /* Lock functions */
    lockMutex();

    if(DEBUG)
        printf("Lets Insert Command\n");

    while(fullQueue(Queue)){
        wait(&waitToNotBeFull);
    }

    insertQueue(Queue, data);
    numberCommands++;
    signal(&waitToNotBeEmpty);
        
    unlockMutex();

    if(DEBUG)
        printf("Inserted Command\n");
    return 1;        
}

char* removeCommand() {
    /* FIXME -> use new functions */
    lockMutex();
    char *removedCommand;
    
    while(emptyQueue(Queue)){
        wait(&waitToNotBeEmpty);
    }

    /* FIXME esta parte nao ta bem, mas nao podemos logo returnar o removeQueue que depois de fazermos removeQueue temos
        de dar signal e unlock */
    removedCommand = removeQueue(Queue);
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

        if(DEBUG)
            printf("Just Scanned\n");

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
    broadcast(&waitToNotBeEmpty);
    switchFinishedState(Queue);

    return NULL;
}

void applyCommands(list* List){
    if(DEBUG)
        printf("Lets Start Applying Commands\n");
        
    while (!getFinishedState(Queue) || !emptyQueue(Queue)){
        const char* command = removeQueue(Queue);

        if (command == NULL){
            continue;
        }

        char token, type;
        char name[MAX_INPUT_SIZE];
        int numTokens = sscanf(command, "%c %s %c", &token, name, &type);
        if (numTokens < 2)
            errorParse("Error: invalid command in Queue\n");

        int searchResult;

        if(DEBUG)
            printf("Lets Start Switch\n");

        switch (token) {
            case 'c':
                switch (type) {
                    case 'f':
                        printf("Create file: %s\n", name);
                        create(name, T_FILE, List);
                        freeItemsList(List, unlockItem);
                        break;
                    case 'd':
                        printf("Create directory: %s\n", name);
                        create(name, T_DIRECTORY, List);
                        freeItemsList(List, unlockItem);
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
                freeItemsList(List, unlockItem);
                break;
            case 'd':
                printf("Delete: %s\n", name);
                delete(name, List);
                freeItemsList(List, unlockItem);
                break;
            default: { /* error */
                errorParse("Error: command to apply\n");
            }
        }
    }
}

void *fnThread(void* arg){
    list* inodeList;

    if(DEBUG)
        printf("Create List\n");

    inodeList = createList();

    if(DEBUG)
        printf("Created List\n");

    applyCommands(inodeList);

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

    if(DEBUG)
        printf("Lets Init Queue\n");

    /* Initialize queue */
    Queue = createQueue();

    if(DEBUG)
        printf("Initialize values\n");

    /* Define Arguments */
    setInitialValues( argv);

    if (numberThreads <= 0)
        /* Error Handling */
        errorParse("Error: Wrong number of threads");

    if(DEBUG)
        printf("Init Mutex\n");

    /* init synch system */
    initLockMutex();
    
    if(DEBUG)
        printf("Init FileSystem\n");

    /* init filesystem */
    init_fs();

    /*starts counting the time*/
    gettimeofday(&tvinicio,NULL);

    if(DEBUG)
        printf("Going to create threads now\n");

    /*creates pool of threads and process input and print tree */
    poolThreads(numberThreads, fnThread, fnThreadProcessInput);

    if(DEBUG)
        printf("Lets Print Tree\n");

    print_tecnicofs_tree(outputFile);
    
    closeFile(outputFile);

    pthread_cond_destroy(&waitToNotBeEmpty);
    
    pthread_cond_destroy(&waitToNotBeFull);

    /* release allocated memory */
    destroy_fs();
    destroyMutex();
    gettimeofday(&tvfinal,NULL);
    printf("TecnicoFS completed in %.4f seconds.\n", (double)(tvfinal.tv_sec - tvinicio.tv_sec) + ((tvfinal.tv_usec - tvinicio.tv_usec)/1000000.0));
    exit(EXIT_SUCCESS);
}
