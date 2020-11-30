#ifndef FH_H
#define FH_H
#include <stdio.h>

FILE *openFile(const char *pathname, const char *mode);
void* closeFile(FILE *stream);

#endif