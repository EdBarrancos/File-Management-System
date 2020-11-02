#include "list.h"

struct _list {
    link head;
    link tail;
};

struct _node{
    Item item;
    link next;
};

list* createList(){
    list *new;
    new = (list*)malloc(sizeof(list));
    new->head = new->tail = NULL;
    return new;
}

void addList(list *List, Item _item){

    link new_node;  
    new_node = (link)malloc(sizeof(node));
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
    
    link current;
    link prev;

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

void freeItemsList(list* List, void (*unlockItem)(Item)){

    link current;
    link prox;

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

link headList(list* List){
    return List->head;
}

Item getNode(link _nextNode){
    return _nextNode->item;
}

link nextNode(link _nextNode){
    return _nextNode->next;
}
