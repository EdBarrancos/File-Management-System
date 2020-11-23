#include "list.h"

struct _list {
    nodeptr head;
    nodeptr tail;
};

struct _node{
    pthread_rwlock_t* item;
    nodeptr next;
};

list* createList(){
    list *new;
    new = (list*)malloc(sizeof(list));
    new->head = new->tail = NULL;
    return new;
}

void addList(list *List, pthread_rwlock_t* _item){

    nodeptr new_node;  
    new_node = (nodeptr)malloc(sizeof(node));
    new_node->item = _item;
    new_node->next = NULL;

    if(emptyList(List)){
        List->head = new_node;
        List->tail = new_node;

        return;
    }
    else{
        (List->tail)->next = new_node;
        List->tail = new_node;
    }

    return;
}

void deleteList(list *List, pthread_rwlock_t* _item){
    
    nodeptr current;
    nodeptr prev;

    for(current=List->head,prev=NULL; current != NULL; prev = current, current = current->next){

        if(current->item == _item){

            if(current==List->head) {
                List->head = current->next;
            }
            else{
                prev->next = current->next;
            }

            if(current==List->tail){
                List->tail=prev;
            }
            free(current);
            break;
        
        }

    }

    return;

}

int searchList(pthread_rwlock_t* itemSearch, list* List){
    nodeptr currentINode;

    currentINode = List->head;

    // Iterate till last element until key is not found
    while(currentINode != NULL && currentINode->item != itemSearch){
        currentINode = currentINode->next;
    }

    if(currentINode == NULL){
        return 0;
    }


    return 1;
}

pthread_rwlock_t* getLastItem(list *List){
    pthread_rwlock_t* lastItem;
    lastItem = List->tail->item;
     
    deleteList(List, lastItem);

    return lastItem;
}

list* freeItemsList(list* List, void (*unlockItem)(pthread_rwlock_t*)){

    nodeptr current;
    nodeptr prox;

    if (!emptyList(List)){

        for(current=List->head,prox=List->head->next; prox!=NULL; current = prox, prox = prox->next){
            unlockItem(current->item);
            free(current);
            }
        }

    unlockItem(current->item);
    free(current);

    List->head = List->tail = NULL;
    return List;

}


void freeList(list* List){
    free(List);
}

int emptyList(list* List){

    return List->head == NULL;

}
