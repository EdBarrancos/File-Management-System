#include "list.h"

struct _lista {
    link head;
    link tail;
};

struct _node{
    Item item;
    link next;
};

list* crialista(){
    list *new;
    new = (list*)malloc(sizeof(list));
    new->head = new->tail = NULL;
    return new;
}

void adicionalista(list *lista, Item elemento){

    link new_node;  
    new_node = (link)malloc(sizeof(node));
    new_node->item = elemento;
    new_node->next = NULL;

    /*se lista for vazia, head e tail apontam para o mesmo elemento*/
    if(lista_vazia(lista)){
        
        lista->head = new_node;
        lista->tail = new_node;

        return;
    }

    /*o node a seguir a tail vai receber o novo elemento e atualiza a tail para esse novo node*/
    lista->tail->next = new_node;
    lista->tail = new_node;

    return;

}

void removelista(list *lista, Item elemento){
    
    link atual;
    link prev;

    /*enquanto nao for atingido o final da lista, corre*/
    for(atual=lista->head,prev=NULL; atual != NULL; prev = atual, atual = atual->next){

        /*caso encontremos o elemento a eliminar*/
        if(atual->item == elemento){

            if(atual==lista->head) {
                /*a head da lista atualiza, pois vai ser eliminado o node que correspondia a head*/
                lista->head = atual->next;
            }
            else{
                /*o next do antecedente do atual passa a ser o que procede o atual*/
                prev->next = atual->next;
            }

            /*caso o atual corresponda a tail, a tail passa a ser o antecedente*/
            if(atual==lista->tail){
                lista->tail=prev;
            }
            
            /*free do node atual*/
            free(atual);
            break;
        
        }

    }

    return;

}

void freelista(list* lista, int controlo, void (*freeitem)(Item)){

    link atual;
    link prox;

    if (!lista_vazia(lista)){

        for(atual=lista->head,prox=lista->head->next; prox!=NULL; atual = prox, prox = prox->next){
            /*caso controlo = 1, free do item*/
            if (controlo == 1){

                freeitem(atual->item);
            }

            free(atual);
        }
        /*libertar o ultimo elemento da lista*/
        /*caso controlo = 1, free do item*/
        if (controlo == 1){
            freeitem(atual->item);
        }
        free(atual);
    }

    free(lista);
    return;
}

int lista_vazia(list* lista){

    return lista->head == NULL;

}

link headlista(list* lista){
    return lista->head;
}

Item lenode(link no){
    return no->item;
}

link nextnode(link no){
    return no->next;
}
