#include "fileHandling.h"
#include <stdio.h>
#include <stdlib.h>


/* Open File return pointer to FILE if success*/
FILE *openFile(const char *pathname, const char *mode){
        FILE *opened = fopen(pathname, mode);

        if(opened == NULL){
            /* Error handling */
            fprintf(stderr, "Error while opening file\n");
            exit(EXIT_FAILURE);
        }

        return opened;
}

/* Closes File */
void closeFile(FILE *stream){
    if(fclose(stream) != 0){
        /* Error Handling */
        fprintf(stderr, "Error while closing file\n");
        exit(EXIT_FAILURE);
    }
}