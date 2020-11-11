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

#define MAX_COMMANDS 150000 /* Passa a 10 */
#define MAX_INPUT_SIZE 100

int numberThreads = 0;

//char inputCommands[MAX_COMMANDS][MAX_INPUT_SIZE];
int numberCommands = 0;
int headQueue = -1;
int tailQueue = -1;

pthread_cond_t *waitToNotBeEmpty, *waitToNotBeFull;


<<<<<<< HEAD
int insertCommand(char* data, queue* Queue) {
    if(numberCommands != MAX_COMMANDS) {
        insert(Queue, data);
=======
int insertCommand(char* data) {
    /* FIXME -> use new functions */
    /* Lock functions */
    if(numberCommands != MAX_COMMANDS) {
        strcpy(inputCommands[numberCommands++], data);
        signal(waitToNotBeEmpty);
>>>>>>> 38d05502546d0780407693f1bff4bdc9c20f464c
        return 1;
    }
    return 0;
}

<<<<<<< HEAD
char* removeCommand(queue* Queue) {
    if(numberThreads > 1)
        lockMutex();
    
    if(numberCommands > 0){
        numberCommands--;
        if(numberThreads > 1)
            unlockMutex();
        return removeQueue(Queue);
//        return inputCommands[headQueue++];  
=======
char* removeCommand() {
    /* FIXME -> use new functions */
    lockMutex();
    
    if(numberCommands > 0){
        numberCommands--;
        signal(waitToNotBeFull);
        unlockMutex();
        return inputCommands[headQueue++];  
>>>>>>> 38d05502546d0780407693f1bff4bdc9c20f464c
    }
    unlockMutex();
    
    return NULL;
}

<<<<<<< HEAD
void processInput(FILE *inputFile, queue* Queue){
=======
void *fnThreadProcessInput(void* arg){
>>>>>>> 38d05502546d0780407693f1bff4bdc9c20f464c
    char line[MAX_INPUT_SIZE];
    FILE *inputFile = (FILE*) arg;
    lockMutex();
    /* break loop with ^Z or ^D */
    while (fgets(line, sizeof(line)/sizeof(char), inputFile)) {
        char token, type;
        char name[MAX_INPUT_SIZE];
        /* FIXME */
        while(/* inputCommand is full */){
            wait(waitToNotBeFull);
        }

        int numTokens = sscanf(line, "%c %s %c", &token, name, &type);

        /* perform minimal validation */
        if (numTokens < 1) {
            continue;
        }
        switch (token) {
            case 'c':
                if(numTokens != 3)
                    errorParse("Error: command invalid\n");
                if(insertCommand(line, Queue))
                    break;
            
            case 'l':
                if(numTokens != 2)
                    errorParse("Error: command invalid\n");
                if(insertCommand(line,Queue))
                    break;
            
            case 'd':
                if(numTokens != 2)
                    errorParse("Error: command invalid\n");
                if(insertCommand(line, Queue))
                    break;
            
            case '#':
                break;
            
            default: { /* error */
                errorParse("Error: command invalid\n");
            }
        }
    }
    closeFile(inputFile);
    unlockMutex();

    return NULL;
}

void applyCommands(list* List, queue* Queue){
    while (numberCommands > 0){
<<<<<<< HEAD
        const char* command = removeCommand(Queue);
=======
        /* FIXME */
        while(/* inputCommand is Empty */)
            wait(waitToNotBeEmpty);

        const char* command = removeCommand();
>>>>>>> 38d05502546d0780407693f1bff4bdc9c20f464c
        if (command == NULL){
            continue;
        }

        char token, type;
        char name[MAX_INPUT_SIZE];
        int numTokens = sscanf(command, "%c %s %c", &token, name, &type);
        if (numTokens < 2)
            errorParse("Error: invalid command in Queue\n");
        int searchResult;
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

void *fnThread(void* arg, queue* Queue){
    list* inodeList;
    inodeList = createList();
    applyCommands(inodeList, Queue);

    return NULL;
}

/*  Argv:
        1 -> inputfile
        2 -> outputfile
        3 -> numThreads */
void setInitialValues(FILE **inputFile, FILE **outputFile, char *argv[]){
    *inputFile = openFile(argv[1], "r");
    *outputFile = openFile(argv[2], "w");
    numberThreads = getNumberThreads(argv[3]);
}

int main(int argc, char* argv[]) {

    struct timeval tvinicio;
    struct timeval tvfinal;
    FILE *inputFile, *outputFile;
    queue* Queue;

    /* Initialize queue */
    Queue = createQueue();

    /* Define Arguments */
    setInitialValues(&inputFile, &outputFile, argv);

    if (numberThreads <= 0)
        /* Error Handling */
        errorParse("Error: Wrong number of threads");

    /* init synch system */
    initMutexLock();
    

    /* init filesystem */
    init_fs();

<<<<<<< HEAD
    /* process input and print tree */
    processInput(inputFile, Queue);

    /*starts counting the time*/
    gettimeofday(&tvinicio,NULL);

    /*creates pool of threads*/
    poolThreads(numberThreads, fnThread, Queue);
=======
    /*starts counting the time*/
    gettimeofday(&tvinicio,NULL);

    /*creates pool of threads and process input and print tree */
    poolThreads(numberThreads, fnThread, fnThreadProcessInput, inputFile);
>>>>>>> 38d05502546d0780407693f1bff4bdc9c20f464c

    print_tecnicofs_tree(outputFile);
    
    closeFile(outputFile);

    /* release allocated memory */
    destroy_fs();
    destroyLock();
    gettimeofday(&tvfinal,NULL);
    printf("TecnicoFS completed in %.4f seconds.\n", (double)(tvfinal.tv_sec - tvinicio.tv_sec) + ((tvfinal.tv_usec - tvinicio.tv_usec)/1000000.0));
    exit(EXIT_SUCCESS);
}
