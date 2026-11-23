/**
 * @file  lista_procesos.h
 * @brief Lista de procesos creados con mkprocess (en orden de creacion).
 */
#ifndef LISTA_PROCESOS_H
#define LISTA_PROCESOS_H

#include "pcb.h"

#define MAX_LISTA 64     /* procesos maximos en la lista */

/**
 * @brief Lista estatica de procesos.
 */
typedef struct {
    PCB datos[MAX_LISTA];       /* procesos en orden de creacion */
    int contador;               /* cantidad de procesos en la lista */
    int siguienteOrden;         /* turno que recibira el proximo alloc */
} ListaProcesos;

void listaInicializar(ListaProcesos *lista);

/** @brief Agrega un proceso en estado NEW. @return 0 si ok, -1 si no. */
int listaAgregar(ListaProcesos *lista, int pid, int burstTime, int tamanoBloques);

/** @brief Busca un proceso por pid. @return el PCB o NULL. */
PCB *listaBuscar(ListaProcesos *lista, int pid);

/** @brief Elimina un proceso conservando el orden. @return 0 si ok, -1 si no existe. */
int listaEliminar(ListaProcesos *lista, int pid);

/** @brief Pasa el proceso a READY y le da su turno en la ready queue. */
void listaMarcarListo(ListaProcesos *lista, PCB *proceso);

/**
 * @brief Procesos en READY ordenados por su llegada a la ready queue.
 * @return Cantidad de procesos guardados en listos.
 */
int listaObtenerListos(ListaProcesos *lista, PCB **listos);

/** @brief Cantidad de procesos en el estado dado. */
int listaContarEstado(const ListaProcesos *lista, EstadoProceso estado);

/** @brief Texto del estado: NEW, READY, RUNNING o TERMINATED. */
const char *estadoTexto(EstadoProceso estado);

/** @brief Color ANSI del estado (vacio si la salida no es una terminal). */
const char *estadoColor(EstadoProceso estado);

void listaMostrar(const ListaProcesos *lista);

#endif /* LISTA_PROCESOS_H */
