#ifndef LISTA_PROCESOS_H
#define LISTA_PROCESOS_H

#include "pcb.h"

#define MAX_LISTA 256

/**
 * @brief Lista estatica para la Ready Queue de procesos.
 * Nomenclatura camelCase: 'datos', 'contador' y 'siguientePid'.
 */
typedef struct {
    PCB datos[MAX_LISTA];       /* 'datos' en espanol */
    int contador;               /* 'contador' en espanol */
    int siguientePid;           /* 'siguientePid' en espanol camelCase */
} ListaProcesos;

/* Alias de compatibilidad */
#define procesos        datos
#define cantidad        contador
#define siguiente_pid   siguientePid

void listaInicializar(ListaProcesos *lista);
int listaAgregar(ListaProcesos *lista, const char *nombre, int burstTime);
void listaMostrar(const ListaProcesos *lista);
void listaVaciar(ListaProcesos *lista);
int listaTieneElementos(const ListaProcesos *lista);

#endif /* LISTA_PROCESOS_H */
