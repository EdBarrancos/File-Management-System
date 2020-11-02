#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>
#include <pthread.h>

#include "fs/operations.h"
#include "fh/fileHandling.h"
#include "thr/threads.h"
#include "er/error.h"

#define MAX_COMMANDS 150000
#define MAX_INPUT_SIZE 100

int numberThreads = 0;

char inputCommands[MAX_COMMANDS][MAX_INPUT_SIZE];
int numberCommands = 0;
int headQueue = 0;


int insertCommand(char* data) {
    if(numberCommands != MAX_COMMANDS) {
        strcpy(inputCommands[numberCommands++], data);
        return 1;
    }
    return 0;
}

char* removeCommand() {
    lockMutex();
    
    if(numberCommands > 0){
        numberCommands--;
        unlockMutex;
        return inputCommands[headQueue++];  
    }

    unlockMutex();
    
    return NULL;
}

void processInput(FILE *inputFile){
    char line[MAX_INPUT_SIZE];
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
                return;
            
            case 'l':
                if(numTokens != 2)
                    errorParse("Error: command invalid\n");
                if(insertCommand(line))
                    break;
                return;
            
            case 'd':
                if(numTokens != 2)
                    errorParse("Error: command invalid\n");
                if(insertCommand(line))
                    break;
                return;
            
            case '#':
                break;
            
            default: { /* error */
                errorParse("Error: command invalid\n");
            }
        }
    }
    closeFile(inputFile);
}

void applyCommands(){
    while (numberCommands > 0){
        const char* command = removeCommand();
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
                        create(name, T_FILE);
                        break;
                    case 'd':
                        printf("Create directory: %s\n", name);
                        create(name, T_DIRECTORY);
                        break;
                    default:
                        errorParse("Error: invalid node type\n");
                }
                break;
            case 'l': 
                searchResult = lookup(name);
                if (searchResult >= 0)
                    printf("Search: %s found\n", name);
                else
                    printf("Search: %s not found\n", name);
                break;
            case 'd':
                printf("Delete: %s\n", name);
                delete(name);
                break;
            default: { /* error */
                errorParse("Error: command to apply\n");
            }
        }
    }
}

void *fnThread(void* arg){
    applyCommands();

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

    /* Define Arguments */
    setInitialValues(&inputFile, &outputFile, argv);

    /* init synch system */
    if(initLock() && numberThreads != 1)
        /* Error Handling */
        errorParse("Error: Incorrect number of threads for a nosync system\n");


    /* init filesystem */
    init_fs();


    /* process input and print tree */
    processInput(inputFile);

    /*starts counting the time*/
    gettimeofday(&tvinicio,NULL);

    /*creates pool of threads*/
    poolThreads(numberThreads, fnThread);

    print_tecnicofs_tree(outputFile);
    
    closeFile(outputFile);

    /* release allocated memory */
    destroy_fs();
    destroyLock();
    gettimeofday(&tvfinal,NULL);
    printf("TecnicoFS completed in %.4f seconds.\n", (double)(tvfinal.tv_sec - tvinicio.tv_sec) + ((tvfinal.tv_usec - tvinicio.tv_usec)/1000000.0));
    exit(EXIT_SUCCESS);
}
