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
    if(pthread_mutex_unlock(&mutexLock))
        /* Error Handling */
        errorParse("Error while Syncing threads with mutex\n");
}

void unlockRW(){
    if(pthread_rwlock_unlock(&rwlockLock))
        /* Error Handling */
        errorParse("Error while Syncing threads with rwlock\n");
}

void lockMutex(){
    if(pthread_mutex_lock(&mutexLock))
        /* Error Handling */
        errorParse("Error while Syncing threads with mutex\n");
}

void lockRWRead(){
    if(pthread_rwlock_rdlock(&rwlockLock))
        /* Error Handling */
        errorParse("Error while Syncing threads with rwlock\n");
}

void lockRWWrite(){
    if(pthread_rwlock_wrlock(&rwlockLock))
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
        return NOSYNC;

    errorParse("Error: syncstrategy wrongly formatted\n");
    return UNKNOWN;
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

    /* If its necessary to force anything, all are initialized */
    pthread_mutex_init(&mutexLock, NULL);
    pthread_rwlock_init(&rwlockLock, NULL);
    switch(syncType){
        case MUTEX:
            return 0;
        case RWLOCK:    
            return 0;
        case NOSYNC:
            return 1;
        case UNKNOWN:
            errorParse("Error: Unkown Sync Strategy\n");
            break;
    }

    /* Supposed to have already returned */
    exit(EXIT_FAILURE); /* Compilador quer que eu devolva alguma cena */
}

void destroyLock(){
    if(pthread_mutex_destroy(&mutexLock))
        /* Error Handling */
        errorParse("Error while destroying mutex lock\n");
    
    if(pthread_rwlock_destroy(&rwlockLock))
        /* Error Handling */
        errorParse("Error while destroying rwlock lock\n");
}

/*  Locks Section of code
    Input
        syncType forceSync
            pass UNKOWN unless required specific lock */
void lockReadSection(syncType forceSync){
    syncType currentSync = syncLock;
    if(forceSync != UNKNOWN && currentSync != NOSYNC)
        currentSync = forceSync;

    switch(currentSync){
        case MUTEX:
            lockMutex();
            break;
        case RWLOCK:    
            lockRWRead();
            break;
        case NOSYNC:
            break;
        case UNKNOWN:
            errorParse("Error: Unkown Sync Strategy\n");
            break;
    }

    /* printf("Lock Read\n"); */
}

/*  Locks Section of code
    Input
        syncType forceSync
            pass UNKOWN unless required specific lock */
void lockWriteSection(syncType forceSync){
    syncType currentSync = syncLock;
    
    if(forceSync != UNKNOWN && currentSync != NOSYNC)
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
        case UNKNOWN:
            errorParse("Error: Unkown Sync Strategy\n");
            break;
    }

    /* printf("Lock write\n"); */
}

/*  unlocks Section of code
    Input
        syncType forceSync
            pass UNKOWN unless required specific lock */
void unlockSection(syncType forceSync){
    syncType currentSync = syncLock;
    if(forceSync != UNKNOWN && currentSync != NOSYNC)
        currentSync = forceSync;

    switch(currentSync){
        case MUTEX:
            unlockMutex();
            break;
        case RWLOCK:    
            unlockRW();
            break;
        case NOSYNC:
            break;
        case UNKNOWN:
            errorParse("Error: Unkown Sync Strategy\n");
            break;
    }

    /* printf("unlock\n"); */
}



