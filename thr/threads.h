#ifndef THR_H
#define THR_H

#include <pthread.h>

typedef enum syncType { UNKNOWN, MUTEX, RWLOCK , NOSYNC} syncType;

syncType syncLock;
pthread_mutex_t mutexLock;
pthread_rwlock_t rwlockLock;

int poolThreads(char* numThreads, void *(*fnThread)());

int initLock(const char *syncType);
void lockReadSection(syncType forceSync);
void lockWriteSection(syncType forceSync);
void unlockSection(syncType forceSync);
#endif