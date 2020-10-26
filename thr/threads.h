#ifndef THR_H
#define THR_H

#include <pthread.h>


void poolThreads(int numberThreads, void *(*fnThread)());
int getNumberThreads(char *numThreads);


int initLock();
void destroyLock();
void lockReadSection();
void lockWriteSection();
void unlockSection();
#endif