#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>
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
    if(numberCommands > 0){
        numberCommands--;
        return inputCommands[headQueue++];  
    }
    return NULL;
}

void processInput(char *filePath){
    char line[MAX_INPUT_SIZE];
    FILE *filePointer = openFile(filePath, "r");
    /* break loop with ^Z or ^D */
    while (fgets(line, sizeof(line)/sizeof(char), filePointer)) {
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
    closeFile(filePointer);
}

void applyCommands(){
    //i zona critica: mutex  
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
    //f zona critica: mutex
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

int main(int argc, char* argv[]) {
    /* init filesystem */
    init_fs();

    /* process input and print tree */
    processInput(argv[1]);
    numberThreads = poolThreads(argv[2], fnThread);
    print_tecnicofs_tree(stdout);

    /* release allocated memory */
    destroy_fs();
    exit(EXIT_SUCCESS);
}
