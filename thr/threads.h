#ifndef THR_H
#define THR_H

#include <pthread.h>
#include "../lst/list.h"


void poolThreads(int numberThreads, void *(*fnThread)());
int getNumberThreads(char *numThreads);

/* Mutex */
void initLockMutex(pthread_mutex_t* lockMutex);
void lockMutex(pthread_mutex_t *lockMutex);
void unlockMutex(pthread_mutex_t *lockMutex);
void destroyMutex(pthread_mutex_t *lockMutex);

/* RW */
void initLockRW(pthread_rwlock_t* lockRW);
void lockReadRW(pthread_rwlock_t *lockRW);
void lockWriteRW(pthread_rwlock_t *lockRW);
void unlockRW(pthread_rwlock_t *lockRW);
void destroyRW(pthread_rwlock_t *lockRW);

void unlockItem (Item _item);

#endif