#ifndef LIST_H
#define LIST_H

#include <stdlib.h>

#define LIST

// Estructura de un nodo de la lista enlazada
typedef struct Node {
  void *data;
  struct Node *next;
} Node;

// Tipo de dato para representar la lista enlazada
typedef Node *List;

// Crea e inicializa una lista vacia (retorna NULL)
LIST List createList(void);

// Elimina todos los nodos de la lista liberando su memoria y opcionalmente los datos
LIST void clearList(List *list, void (*freeData)(void *));

// Destruye la lista y libera la memoria acumulada
LIST void destroyList(List *list, void (*freeData)(void *));

// Inserta un elemento al inicio de la lista
LIST int listInsertBegin(List *list, void *data);

// Inserta un elemento al final de la lista
LIST int listInsertEnd(List *list, void *data);

// Inserta un elemento en una posicion especifica de la lista
LIST int listInsertAt(List *list, int index, void *data);

// Remueve y retorna el dato del primer elemento de la lista
LIST void *listRemoveBegin(List *list);

// Remueve y retorna el dato del ultimo elemento de la lista
LIST void *listRemoveEnd(List *list);

// Remueve y retorna el dato del elemento en el indice especificado
LIST void *listRemove(List *list, int index);

// Obtiene el dato en el indice especificado sin removerlo
LIST void *getList(List list, int index);

// Reemplaza el dato en el indice especificado y retorna el dato anterior
LIST void *setList(List *list, int index, void *data);

// Retorna la cantidad de elementos en la lista
LIST int sizeList(List list);

// Verifica si la lista esta vacia (1 si esta vacia, 0 en caso contrario)
LIST int listIsEmpty(List list);

// Retorna el indice del elemento buscando con una funcion comparadora (-1 si no lo encuentra)
LIST int listIndexOf(List list, void *data, int (*cmp)(void *, void *));

// Verifica si la lista contiene un elemento (1 si existe, 0 en caso contrario)
LIST int listContains(List list, void *data, int (*cmp)(void *, void *));

#endif