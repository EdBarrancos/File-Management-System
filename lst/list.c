#include "list.h"

struct _list {
    nodeptr head;
    nodeptr tail;
};

struct _node{
    Item item;
    nodeptr next;
};

list* createList(){
    list *new;
    new = (list*)malloc(sizeof(list));
    new->head = new->tail = NULL;
    return new;
}

void addList(list *List, Item _item){

    nodeptr new_node;  
    new_node = (nodeptr)malloc(sizeof(node));
    new_node->item = _item;
    new_node->next = NULL;

    if(emptyList(List)){
        
        List->head = new_node;
        List->tail = new_node;

        return;
    }

    List->tail->next = new_node;
    List->tail = new_node;

    return;

}

void deleteList(list *List, Item _item){
    
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

void getLastItem(list *List){
    Item lastItem;
    lastItem = List->tail->item;
     
    deleteList(List, lastItem);

    return lastItem;
}

void freeItemsList(list* List, void (*unlockItem)(Item)){

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

    return;

}


void freeList(list* List){
    free(List);
}

int emptyList(list* List){

    return List->head == NULL;

}

nodeptr headList(list* List){
    return List->head;
}

Item getNode(nodeptr _nextNode){
    return _nextNode->item;
}

nodeptr nextNode(nodeptr _nextNode){
    return _nextNode->next;
}
