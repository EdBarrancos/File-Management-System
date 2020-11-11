#ifndef _QUEUE_ 
#define _QUEUE_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 10 //max number of elements
#define MAX_INPUT_SIZE 100 

/*
 * */
typedef struct _queue queue;

/*
 * creates new queue
 * index of start of queue and end of queue both start at i=-1
 * returns queue*/
queue* createQueue();

/*
 * checks if queue is empty*/
int emptyQueue(queue* Queue);

/*
 * checks if queue if full */
int fullQueue(queue* Queue);

/*
 * insert element in queue 
 * returns 0 if queue is full and 1 in case of success*/
int insertQueue(queue* Queue, char* element);

/*
 * remove element from queue */
char* removeQueue(queue* Queue);

/*
 *free queue*/
void freeQueue(queue* Queue);

/*
 *display queue*/
void displayQueue(queue* Queue);

#endif
