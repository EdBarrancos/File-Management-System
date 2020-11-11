#include "threads.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../er/error.h"
#include "../cq/circularqueue.h"

/*  Creates threads and associates tasks
    Input
        char* numThreads -> Number of threads to be created
        void *(*fnThread)() -> Pointer to the functions to be executed */
<<<<<<< HEAD
void poolThreads(int numberThreads, void *(*fnThread)(), queue* Queue){
=======
void poolThreads(int numberThreads, void *(*fnThread)(), void *(*fnThreadProcessInput)(), FILE* inputFile){
>>>>>>> 38d05502546d0780407693f1bff4bdc9c20f464c
    
    pthread_t tid[numberThreads];
    pthread_t inputProcessor;
    int i;

    if(pthread_create(&inputProcessor, NULL, fnThreadProcessInput, (void *) inputFile))
        /* Error Handling */
        errorParse("Error while creating task.\n");

    for (i=0; i<numberThreads; i++){
        if (pthread_create(&tid[i], NULL, fnThread(Queue), NULL)!=0)
            /* Error Handling */
            errorParse("Error while creating task.\n");
    }

    if(pthread_join(inputProcessor, NULL))
        /* Error Handling */
        errorParse("Error while joining threads.\n");

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


/* ************************
******  MUTEX FUNCTIONS  **
************************  */


void initLockMutex(){
    if(pthread_mutex_init(lockM, NULL))
        /* Error Handling */
        errorParse("Error while Initing Mutex\n");
}

void lockMutex(){
    if(pthread_mutex_lock(lockM))
        /* Error Handling */
        errorParse("Error while locking mutex\n");
}

void unlockMutex(){
     if(pthread_mutex_lock(lockM))
        /* Error Handling */
        errorParse("Error while unlocking mutex\n");
}

void destroyMutex(){
    if(pthread_mutex_destroy(lockM))
        /* Error Handling */
        errorParse("Error while destroying mutex lock\n");
}

void wait(pthread_cond_t *varCond){
    if(pthread_cond_wait(varCond, lockM))
        /* Error Handling */
        errorParse("Error while waiting");
}

void signal(pthread_cond_t *varCond){
    if(pthread_cond_signal(varCond))
        /* Error handling */
        errorParse("Error while signaling");
}


/* ************************
******  RW FUNCTIONS  **
************************  */



void initLockRW(pthread_rwlock_t* lockRW){
    if(pthread_rwlock_init(lockRW, NULL))
        /* Error Handling */
        errorParse("Error while Initing RWlock\n");
}

void lockReadRW(pthread_rwlock_t *lockRW){
    if (pthread_rwlock_rdlock(lockRW))
        /* Error Handling */
        errorParse("Error while locking read rwlock\n");
}

void lockWriteRW(pthread_rwlock_t *lockRW){
    if (pthread_rwlock_wrlock(lockRW))
        /* Error handling */
        errorParse("Error while locking write rwlock\n");
}

void unlockRW(pthread_rwlock_t *lockRW){
    if (pthread_rwlock_unlock(lockRW))
        /* Error Handling */
        errorParse("Error while unlocking rwlock\n");
}

void destroyRW(pthread_rwlock_t *lockRW){
    if(pthread_rwlock_destroy(lockRW))
        /* Error Handling */
        errorParse("Error while destroying rwlock lock\n");
}

/*
* unlock each inode of list*/

void unlockItem (Item _item){
    /*transformar Item em ptr_jogo, para aceder a nome_jogo*/
    pthread_rwlock_t* lockINode;
    lockINode = (pthread_rwlock_t*)_item;
    unlockRW(lockINode);
}
