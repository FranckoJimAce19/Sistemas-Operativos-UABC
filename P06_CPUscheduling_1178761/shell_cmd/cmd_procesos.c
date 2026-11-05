#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmd_procesos.h"
#include "../scheduler/fcfs.h"
#include "../scheduler/sjf.h"
#include "../scheduler/rr.h"

void cmdMkprocess(ListaProcesos *lista, char **args) {
    if (!lista) return;
    if (!args[1] || !args[2]) {
        printf("Uso: mkprocess [nombre] [burstTime]\n");
        printf("Ejemplo: mkprocess P1 10\n");
        return;
    }
    char *nombre = args[1];
    int burstTime = atoi(args[2]);
    if (burstTime <= 0) {
        printf("Error: el burstTime debe ser un numero mayor a 0.\n");
        return;
    }
    int pid = listaAgregar(lista, nombre, burstTime);
    if (pid >= 0) {
        printf("Proceso '%s' creado con PID %d y burst time %d\n", nombre, pid, burstTime);
    }
}

void cmdLstprocss(ListaProcesos *lista) {
    if (!lista) return;
    listaMostrar(lista);
}

void cmdFcfs(ListaProcesos *lista) {
    if (!lista) return;
    ejecutarFCFS(lista);
}

void cmdSjf(ListaProcesos *lista) {
    if (!lista) return;
    ejecutarSJF(lista);
}

void cmdRr(ListaProcesos *lista) {
    if (!lista) return;
    ejecutarRR(lista);
}
