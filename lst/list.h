#ifndef _LIST_ 
#define _LIST_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
*void pointer to each item of the list*/
typedef void* Item;

/*
* stuct list:                                                                
*   -head pointer                                                        
*   -tail pointer*/

typedef struct _list list;

/*
* node of list:                                                     
*   -pointer void to an item                                        
*   -pointer node to next node*/
typedef struct _node node;

/*
* pointer to node*/

typedef node* nodeptr;

/*
* initializes list, head e tail point to NULL                       
*   -returns pointer to initialized list*/
list* createList();

/*
* adds a new item to the list*/

void addList(list *List, Item _item);

/*
* deletes an item of list*/
void deleteList(list *List, Item _item);

/*
* checks if list is empty                                           
*   -returns 1 if empty                                             
*   -retorns 0 otherwise*/
int emptyList(list* List);

/*
* free items of List */
void freeItemsList(list* List, void (*unlockItem)(Item));

/*
* free list*/
void freeList();


/*
* pointer to head of list*/
nodeptr headList(list* List);

/*
* pointer to item of list*/
Item getNode(nodeptr node);

/*
* returns pointer to next item*/

nodeptr nextNode(nodeptr _nextNode);

#endif
