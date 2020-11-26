#ifndef THR_H
#define THR_H

#include <pthread.h>
#include "../lst/list.h"


void poolThreads(int numberThreads, void *(*fnThread)());
int getNumberThreads(char *numThreads);

/* Mutex */
void initLockMutex();
void lockMutex();
void unlockMutex();
void destroyMutex();
void wait(pthread_cond_t *varCond);
void signal(pthread_cond_t *varCond);
void broadcast(pthread_cond_t *varCond);

/* RW */
void initLockRW(pthread_rwlock_t* lockRW);
void lockReadRW(pthread_rwlock_t *lockRW);
void lockWriteRW(pthread_rwlock_t *lockRW);
int tryLockRead(pthread_rwlock_t *lockRW);
int tryLockWrite(pthread_rwlock_t *lockRW);
void unlockRW(pthread_rwlock_t *lockRW);
void destroyRW(pthread_rwlock_t *lockRW);

void unlockItem (pthread_rwlock_t* _item);

#endif