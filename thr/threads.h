#ifndef THR_H
#define THR_H

#include <pthread.h>

typedef enum syncType { UNKNOWN, MUTEX, RWLOCK , NOSYNC} syncType;

syncType syncLock;
pthread_mutex_t mutexLock;
pthread_rwlock_t rwlockLock;

void poolThreads(int numberThreads, void *(*fnThread)());
int getNumberThreads(char *numThreads);

syncType getSyncType(const char *syncTypeString);
int initLock(syncType syncType);
void lockReadSection(syncType forceSync);
void lockWriteSection(syncType forceSync);
void unlockSection(syncType forceSync);
#endif