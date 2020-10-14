#include "threads.h"
#include <pthread.h>
#include <stdio.h>
#include "../er/error.h"

/*  Creates threads and associates tasks
    Input
        char* numThreads -> Number of threads to be created
        void +(*fnThread)() -> Pointer to the functions to be executed
    Returns 
        int -> number of Threads */
int poolThreads(char* numThreads, void *(*fnThread)()){
    int numberThreads = atoi(numThreads);
    pthread_t tid[numberThreads];
    int i;

    if (!numberThreads)
        /* Error Handling */
        errorParse( "Error Number of Threads Wrongly Formatted\n");

    for (i=0; i<numberThreads; i++){
        if (pthread_create(&tid[i], NULL, fnThread, NULL)!=0)
            /* Error Handling */
            errorParse("Error while waiting for task.\n");
    }

    for (i=0; i<numberThreads; i++){
        pthread_join(tid[i], NULL);
    }

    return numberThreads;
}