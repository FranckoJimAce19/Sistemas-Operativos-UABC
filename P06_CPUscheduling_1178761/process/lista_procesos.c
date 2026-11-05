#include <stdio.h>
#include <string.h>
#include "lista_procesos.h"

void listaInicializar(ListaProcesos *lista) {
    if (!lista) return;
    lista->contador = 0;
    lista->siguientePid = 1;
}

int listaAgregar(ListaProcesos *lista, const char *nombre, int burstTime) {
    if (!lista || !nombre) return -1;
    if (lista->contador >= MAX_LISTA) {
        printf("Error: lista de procesos llena (maximo %d procesos).\n", MAX_LISTA);
        return -1;
    }
    if (burstTime <= 0) {
        printf("Error: el burst time debe ser mayor a 0.\n");
        return -1;
    }
    
    PCB *p = &lista->datos[lista->contador];
    p->pid = lista->siguientePid++;
    strncpy(p->nombre, nombre, MAX_PROC_NAME - 1);
    p->nombre[MAX_PROC_NAME - 1] = '\0';
    p->burstTime = burstTime;
    p->remainingTime = burstTime;
    p->arrivalTime = 0;
    p->startTime = -1;
    p->finishTime = -1;
    p->waitingTime = 0;
    p->turnaroundTime = 0;
    p->estado = ESTADO_LISTO;
    
    lista->contador++;
    return p->pid;
}

void listaMostrar(const ListaProcesos *lista) {
    if (!lista || lista->contador == 0) {
        printf("No hay procesos en la lista.\n");
        return;
    }
    printf("\n%-5s %-20s %-12s %-12s\n", "PID", "NOMBRE", "BURST TIME", "ESTADO");
    printf("------------------------------------------------------------\n");
    for (int i = 0; i < lista->contador; i++) {
        const PCB *p = &lista->datos[i];
        const char *estadoStr = "LISTO";
        if (p->estado == ESTADO_EJECUCION) estadoStr = "EJECUCION";
        else if (p->estado == ESTADO_TERMINADO) estadoStr = "TERMINADO";
        else if (p->estado == ESTADO_NUEVO) estadoStr = "NUEVO";
        printf("%-5d %-20s %-12d %-12s\n", p->pid, p->nombre, p->burstTime, estadoStr);
    }
    printf("------------------------------------------------------------\n\n");
}

void listaVaciar(ListaProcesos *lista) {
    if (!lista) return;
    lista->contador = 0;
    lista->siguientePid = 1;
}

int listaTieneElementos(const ListaProcesos *lista) {
    return (lista != NULL && lista->contador > 0);
}
