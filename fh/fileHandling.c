#include "fileHandling.h"
#include <stdio.h>
#include "../er/error.h"

/* Open File return pointer to FILE if success*/
FILE *openFile(const char *pathname, const char *mode){
        FILE *opened = fopen(pathname, mode);

        if(opened == NULL)
            /* Error handling */
            errorParse("Error while opening file\n");

        return opened;
}

/* Closes File */
void closeFile(FILE *stream){
    if(fclose(stream) != 0)
        /* Error Handling */
        errorParse("Error while closing file\n");
}