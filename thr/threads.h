#ifndef THR_H
#define THR_H

#include <pthread.h>

int poolThreads(char* numThreads, void *(*fnThread)());

#endif