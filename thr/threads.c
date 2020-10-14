#include "threads.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../er/error.h"

/*  Creates threads and associates tasks
    Input
        char* numThreads -> Number of threads to be created
        void *(*fnThread)() -> Pointer to the functions to be executed */
void poolThreads(int numberThreads, void *(*fnThread)()){
    
    pthread_t tid[numberThreads];
    int i;

    for (i=0; i<numberThreads; i++){
        if (pthread_create(&tid[i], NULL, fnThread, NULL)!=0)
            /* Error Handling */
            errorParse("Error while waiting for task.\n");
    }

    for (i=0; i<numberThreads; i++){
        if(pthread_join(tid[i], NULL))
            /* Error Handling */
            errorParse("Error while joining the threads\n");
    }
}

int getNumberThreads(char *numThreads){
    int numberThreads = atoi(numThreads);

    if (!numberThreads)
        /* Error Handling */
        errorParse( "Error Number of Threads Wrongly Formatted\n");

    return numberThreads;
}

/* Locks And Unlocks for Mutex and Rwlock */
void unlockMutex(){
    if(!pthread_mutex_unlock(&mutexLock))
        /* Error Handling */
        errorParse("Error while Syncing threads with mutex\n");
}

void unlockRW(){
    if(!pthread_rwlock_unlock(&rwlockLock))
        /* Error Handling */
        errorParse("Error while Syncing threads with rwlock\n");
}

void lockMutex(){
    if(!pthread_mutex_lock(&mutexLock))
        /* Error Handling */
        errorParse("Error while Syncing threads with mutex\n");
}

void lockRWRead(){
    if(!pthread_rwlock_rdlock(&rwlockLock))
        /* Error Handling */
        errorParse("Error while Syncing threads with rwlock\n");
}

void lockRWWrite(){
    if(!pthread_rwlock_wrlock(&rwlockLock))
        /* Error Handling */
        errorParse("Error while Syncing threads with rwlock\n");
}
/* Locks And Unlocks for Mutex and Rwlock */

syncType getSyncType(const char *syncTypeString){
    if(!strcmp(syncTypeString, "mutex"))
        return MUTEX;
    else if(!strcmp(syncTypeString, "rwlock"))
        return RWLOCK;
    else if(!strcmp(syncTypeString, "nosync"))
        syncLock = NOSYNC;
    else{
        errorParse("Error: syncstrategy wrongly formatted\n");
        return UNKNOWN;
    }
}

/*  Input
        cont char *syncType -> defines the type of sync
            mutex
            rwlock
            nosync
    Output
        0 if type is either  mutex or rwlock 
        1 if type is nosync */
int initLock(syncType syncType){
    syncLock = syncType;

    pthread_mutex_init(&mutexLock, NULL); /* Has to be initialized for the applyCommands */
    switch(syncType){
        case MUTEX:
            return 0;
        case RWLOCK:    
            pthread_rwlock_init(&rwlockLock, NULL);
            return 0;
        case NOSYNC:
            return 1;
    }
}

/*  Locks Section of code
    Input
        syncType forceSync
            pass NULL unless required specific lock */
void lockReadSection(syncType forceSync){
    syncType currentSync = syncLock;
    if(forceSync != UNKNOWN)
        currentSync = forceSync;

    switch(currentSync){
        case MUTEX:
            lockMutex();
            break;
        case RWLOCK:    
            lockRWWrite();
            break;
        case NOSYNC:
            break;
    }
}

/*  Locks Section of code
    Input
        syncType forceSync
            pass NULL unless required specific lock */
void lockWriteSection(syncType forceSync){
    syncType currentSync = syncLock;
    if(forceSync != UNKNOWN)
        currentSync = forceSync;

    switch(currentSync){
        case MUTEX:
            lockMutex();
            break;
        case RWLOCK:    
            lockRWWrite();
            break;
        case NOSYNC:
            break;
    }
}

/*  unlocks Section of code
    Input
        syncType forceSync
            pass NULL unless required specific lock */
void unlockSection(syncType forceSync){
    syncType currentSync = syncLock;
    if(forceSync != UNKNOWN)
        currentSync = forceSync;

    switch(currentSync){
        case MUTEX:
            unlockMutex();
        case RWLOCK:    
            unlockRW();
        case NOSYNC:
            break;
    }
}



