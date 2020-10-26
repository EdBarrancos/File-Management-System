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



void initLockMutex(pthread_mutex_t* lockMutex){
    if(pthread_mutex_init(lockMutex, NULL))
        /* Error Handling */
        errorParse("Error while Initing Mutex\n");
}

void initLockRW(pthread_rwlock_t* lockRW){
    if(pthread_rwlock_init(lockRW, NULL))
        /* Error Handling */
        errorParse("Error while Initing RWlock\n");
}


void destroyLock(){
    // if(pthread_mutex_destroy(&mutexLock))
    //     /* Error Handling */
    //     errorParse("Error while destroying mutex lock\n");
    
    // if(pthread_rwlock_destroy(&rwlockLock))
    //     /* Error Handling */
    //     errorParse("Error while destroying rwlock lock\n");
}

/*  Locks Section of code
    Input
        syncType forceSync
            pass UNKOWN unless required specific lock */
void lockReadSection(){

    // switch(currentSync){
    //     case MUTEX:
    //         lockMutex();
    //         break;
    //     case RWLOCK:    
    //         lockRWRead();
    //         break;
    //     case NOSYNC:
    //         break;
    //     case UNKNOWN:
    //         errorParse("Error: Unkown Sync Strategy\n");
    //         break;
    // }

    // /* printf("Lock Read\n"); */
}

/*  Locks Section of code
    Input
        syncType forceSync
            pass UNKOWN unless required specific lock */
void lockWriteSection(){

    // switch(currentSync){
    //     case MUTEX:
    //         lockMutex();
    //         break;
    //     case RWLOCK:    
    //         lockRWWrite();
    //         break;
    //     case NOSYNC:
    //         break;
    //     case UNKNOWN:
    //         errorParse("Error: Unkown Sync Strategy\n");
    //         break;
    // }

    // /* printf("Lock write\n"); */
}

/*  unlocks Section of code
    Input
        syncType forceSync
            pass UNKOWN unless required specific lock */
void unlockSection(){

    // switch(currentSync){
    //     case MUTEX:
    //         unlockMutex();
    //         break;
    //     case RWLOCK:    
    //         unlockRW();
    //         break;
    //     case NOSYNC:
    //         break;
    //     case UNKNOWN:
    //         errorParse("Error: Unkown Sync Strategy\n");
    //         break;
    // }

    // /* printf("unlock\n"); */
}



