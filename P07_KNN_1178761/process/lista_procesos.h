/**
 * @file  lista_procesos.h
 * @brief Lista de procesos listos (ready queue) — Practica 6 SO.
 */
#ifndef LISTA_PROCESOS_H
#define LISTA_PROCESOS_H

#include "pcb.h"

#define MAX_LISTA 256

/**
 * @brief Lista estatica para la Ready Queue de procesos.
 */
typedef struct {
    PCB datos[MAX_LISTA];       /* procesos en orden de llegada */
    int contador;               /* cantidad de procesos en la lista */
    int siguientePid;           /* PID que recibira el proximo proceso */
} ListaProcesos;

void listaInicializar(ListaProcesos *lista);
int listaAgregar(ListaProcesos *lista, const char *nombre, int burstTime);
void listaMostrar(const ListaProcesos *lista);
void listaVaciar(ListaProcesos *lista);

#endif /* LISTA_PROCESOS_H */
