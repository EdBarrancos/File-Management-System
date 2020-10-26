#ifndef THR_H
#define THR_H

#include <pthread.h>


void poolThreads(int numberThreads, void *(*fnThread)());
int getNumberThreads(char *numThreads);


void initLockMutex(pthread_mutex_t* lockMutex);
void initLockRW(pthread_rwlock_t* lockRW);
void destroyLock();
void lockReadSection();
void lockWriteSection();
void unlockSection();
#endif