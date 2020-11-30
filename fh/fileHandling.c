#include "fileHandling.h"
#include <stdio.h>
#include "../er/error.h"

#define FAIL 1

/* Open File return pointer to FILE if success*/
FILE *openFile(const char *pathname, const char *mode){
        FILE *opened = fopen(pathname, mode);

        if(!opened)
            /* Error handling */
            return FAIL;

        return opened;
}

/* Closes File */
int closeFile(FILE *stream){
    if(fclose(stream))
        /* Error Handling */
        return FAIL;
}