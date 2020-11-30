#include "fileHandling.h"
#include <stdio.h>
#include "../er/error.h"

/* Open File return pointer to FILE if success*/
FILE *openFile(const char *pathname, const char *mode){
        FILE *opened = fopen(pathname, mode);

        if(!opened)
            /* Error handling */
            return NULL;

        return opened;
}

/* Closes File */
void* closeFile(FILE *stream){
    if(fclose(stream))
        /* Error Handling */
        return NULL;
    else
        return (void*) 1;
}