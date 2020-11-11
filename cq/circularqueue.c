#include "circularqueue.h"

struct _queue {
    int head;
    int tail;
    char inputCommands[MAX][MAX_INPUT_SIZE];
};


/*
 * creates new queue*/
queue* createQueue(){
    queue *new;
    new = (queue*)malloc(sizeof(queue));
    new->head = new->tail = -1;
    return new;
}

/*
 * checks if queue is empty*/

int emptyQueue(queue* Queue){
    if (Queue->tail == -1){
        return 1;
    }

    return 0;
}

/*
 * checks if queue if full */
int fullQueue(queue* Queue){

    if((Queue->head == Queue->tail + 1) || (Queue->head == 0 && Queue->tail == MAX - 1)){
        return 1;
    } 

    return 0;	

}

/*
 * insert element in queue 
 * returns 0 if queue is full and 1 in case of success*/
int insertQueue(queue* Queue, char* element){

	// checks if queue is full
	if(fullQueue(Queue)){
		return 0;
	}

    else{
        if(Queue->head == -1){
            Queue->head = 0;
        }

        Queue->tail = (Queue->tail + 1) % MAX;

        strcpy(Queue->inputCommands[Queue->tail], element);

        return 1;

    }
	
}

/*
 * remove element from queue */
char* removeQueue(queue* Queue){
	
    char* element;

    if(!emptyQueue(Queue)){

        element = Queue->inputCommands[Queue->head];

        if(Queue->head == Queue->tail){
            Queue->head = -1;
            Queue->tail = -1;
        }

        else{
            Queue->head = (Queue->head + 1) % MAX;
        }
        return element;
    }

    else{

        return NULL;

    }
}

/*
 *free queue*/
void freeQueue(queue* Queue){
    free(Queue);
}

/*
 *display queue*/
void displayQueues(queue* Queue) {
  int i;
  if (emptyQueue(Queue))
    printf(" \n Empty Queue\n");
  else {
    printf("\n Front -> %d ", Queue->head);
    printf("\n Items -> ");
    for (i = Queue->head; i != Queue->tail; i = (i + 1) % MAX) {
      printf("%s ", Queue->inputCommands[i]);
    }
    printf("%s ", Queue->inputCommands[i]);
    printf("\n Rear -> %d \n", Queue->tail);
  }
}



