#include "List.h"

// Crea un nodo interno para la lista
static Node *createNode(void *data) {
  Node *node = (Node *)malloc(sizeof(Node));
  if (node == NULL) {
    return NULL;
  }
  node->data = data;
  node->next = NULL;
  return node;
}

// Crea e inicializa una lista vacia (retorna NULL)
LIST List createList(void) { return NULL; }

// Elimina todos los nodos de la lista liberando su memoria y opcionalmente los
// datos
LIST void clearList(List *list, void (*freeData)(void *)) {
  Node *current;
  Node *nextNode;
  if (list == NULL) {
    return;
  }
  current = *list;
  while (current != NULL) {
    nextNode = current->next;
    if (freeData != NULL && current->data != NULL) {
      freeData(current->data);
    }
    free(current);
    current = nextNode;
  }
  *list = NULL;
}

// Destruye la lista y libera la memoria acumulada
LIST void destroyList(List *list, void (*freeData)(void *)) {
  clearList(list, freeData);
}

// Inserta un elemento al inicio de la lista
LIST int listInsertBegin(List *list, void *data) {
  Node *node;
  if (list == NULL) {
    return -1;
  }
  node = createNode(data);
  if (node == NULL) {
    return -1;
  }
  node->next = *list;
  *list = node;
  return 0;
}

// Inserta un elemento al final de la lista
LIST int listInsertEnd(List *list, void *data) {
  Node *node;
  Node *currentNode;
  if (list == NULL) {
    return -1;
  }
  node = createNode(data);
  if (node == NULL) {
    return -1;
  }
  if (*list == NULL) {
    *list = node;
    return 0;
  }
  currentNode = *list;
  while (currentNode->next != NULL) {
    currentNode = currentNode->next;
  }
  currentNode->next = node;
  return 0;
}

// Inserta un elemento en una posicion especifica de la lista
LIST int listInsertAt(List *list, int index, void *data) {
  Node *node;
  Node *current;
  int i;
  if (list == NULL || index < 0) {
    return -1;
  }
  if (index == 0) {
    return listInsertBegin(list, data);
  }
  current = *list;
  for (i = 0; i < index - 1; i++) {
    if (current == NULL) {
      return -1;
    }
    current = current->next;
  }
  if (current == NULL) {
    return -1;
  }
  node = createNode(data);
  if (node == NULL) {
    return -1;
  }
  node->next = current->next;
  current->next = node;
  return 0;
}

// Remueve y retorna el dato del primer elemento de la lista
LIST void *listRemoveBegin(List *list) {
  Node *first;
  void *data;
  if (list == NULL || *list == NULL) {
    return NULL;
  }
  first = *list;
  data = first->data;
  *list = first->next;
  free(first);
  return data;
}

// Remueve y retorna el dato del ultimo elemento de la lista
LIST void *listRemoveEnd(List *list) {
  Node *current;
  Node *previous;
  void *data;
  if (list == NULL || *list == NULL) {
    return NULL;
  }
  if ((*list)->next == NULL) {
    data = (*list)->data;
    free(*list);
    *list = NULL;
    return data;
  }
  previous = *list;
  current = previous->next;
  while (current->next != NULL) {
    previous = current;
    current = current->next;
  }
  data = current->data;
  previous->next = NULL;
  free(current);
  return data;
}

// Remueve y retorna el dato del elemento en el indice especificado
LIST void *listRemove(List *list, int index) {
  Node *current;
  Node *previous;
  void *data;
  int i;
  if (list == NULL || *list == NULL || index < 0) {
    return NULL;
  }
  if (index == 0) {
    return listRemoveBegin(list);
  }
  previous = *list;
  current = previous->next;
  i = 1;
  while (current != NULL && i < index) {
    previous = current;
    current = current->next;
    i++;
  }
  if (current == NULL) {
    return NULL;
  }
  data = current->data;
  previous->next = current->next;
  free(current);
  return data;
}

// Obtiene el dato en el indice especificado sin removerlo
LIST void *getList(List list, int index) {
  Node *current;
  int i;
  if (list == NULL || index < 0) {
    return NULL;
  }
  current = list;
  for (i = 0; i < index; i++) {
    if (current->next == NULL) {
      return NULL;
    }
    current = current->next;
  }
  return current->data;
}

// Reemplaza el dato en el indice especificado y retorna el dato anterior
LIST void *setList(List *list, int index, void *data) {
  Node *current;
  void *oldData;
  int i;
  if (list == NULL || *list == NULL || index < 0) {
    return NULL;
  }
  current = *list;
  for (i = 0; i < index; i++) {
    if (current->next == NULL) {
      return NULL;
    }
    current = current->next;
  }
  oldData = current->data;
  current->data = data;
  return oldData;
}

// Retorna la cantidad de elementos en la lista
LIST int sizeList(List list) {
  int count = 0;
  Node *current = list;
  while (current != NULL) {
    count++;
    current = current->next;
  }
  return count;
}

// Verifica si la lista esta vacia (1 si esta vacia, 0 en caso contrario)
LIST int listIsEmpty(List list) { return (list == NULL) ? 1 : 0; }

// Retorna el indice del elemento buscando con una funcion comparadora (-1 si no
// lo encuentra)
LIST int listIndexOf(List list, void *data, int (*cmp)(void *, void *)) {
  Node *current;
  int index;
  if (cmp == NULL) {
    return -1;
  }
  current = list;
  index = 0;
  while (current != NULL) {
    if (cmp(current->data, data) == 0) {
      return index;
    }
    current = current->next;
    index++;
  }
  return -1;
}

// Verifica si la lista contiene un elemento (1 si existe, 0 en caso contrario)
LIST int listContains(List list, void *data, int (*cmp)(void *, void *)) {
  return (listIndexOf(list, data, cmp) >= 0) ? 1 : 0;
}