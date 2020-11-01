#ifndef _LISTA_ 
#define _LISTA_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*-------------------------------------------------------------------*/
/*pointer void para o item de cada node                              */
/*-------------------------------------------------------------------*/
typedef void* Item;

/*-------------------------------------------------------------------*/
/*stuct list, com o pointer da head e da tail da mesma               */
/*-------------------------------------------------------------------*/
typedef struct _lista list;

/*-------------------------------------------------------------------*/
/* node da lista, constituido por um pointer void para o item e um   */
/* pointer node para o proximo node                                  */
/*-------------------------------------------------------------------*/
typedef struct _node node;

/*-------------------------------------------------------------------*/
/* pointer para node de lista                                        */
/*-------------------------------------------------------------------*/
typedef node* link;

/*-------------------------------------------------------------------*/
/* cria lista, head e tail apontam para NULL                         */
/* -retorna pointer para lista criada                                */
/*-------------------------------------------------------------------*/
list* crialista();

/*-------------------------------------------------------------------*/
/* adiciona novo elemento ao final da lista                          */
/*-------------------------------------------------------------------*/
void adicionalista(list *lista, Item elemento);

/*-------------------------------------------------------------------*/
/* remove um elemento da lista                                       */
/*-------------------------------------------------------------------*/
void removelista(list *lista, Item elemento);

/*-------------------------------------------------------------------*/
/* verifica se a lista esta vazia                                    */
/*                                                                   */
/* -retorna 1 caso a lista estiver vazia                             */
/* -retorna 0 caso a lista nao estiver vazia                         */
/*-------------------------------------------------------------------*/
int lista_vazia(list* lista);

/*-------------------------------------------------------------------*/
/* free da lista                                                     */  
/*                                                                   */
/* -controlo=1: free da lista e dos seus items                       */
/* -freeitem: corresponde a funcao para dar free ao item da lista    */
/* -controlo=0: free da lista, mas nao dos items da mesma            */
/*-------------------------------------------------------------------*/
void freelista(list* lista, int controlo, void (*freeitem)(Item));

/*-------------------------------------------------------------------*/
/*retorna pointer para a head de lista dada                          */
/*-------------------------------------------------------------------*/
link headlista(list* lista);

/*-------------------------------------------------------------------*/
/*retorna pointer para o item de node dado                           */
/*-------------------------------------------------------------------*/
Item lenode(link node);

/*-------------------------------------------------------------------*/
/*retorna pointer para o proximo node do node dado                   */
/*-------------------------------------------------------------------*/
link nextnode(link no);

#endif
