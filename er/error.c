#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void errorParse(const char *error){
    fprintf(stderr,"%s", error);
    exit(EXIT_FAILURE);
}