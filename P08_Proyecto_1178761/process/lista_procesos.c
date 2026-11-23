/**
 * @file  lista_procesos.c
 * @brief Lista de procesos creados con mkprocess (en orden de creacion).
 */
#include <stdio.h>
#include <string.h>
#include "lista_procesos.h"
#include "../memory/memoria.h"
#include "../utilidades/terminal.h"

void listaInicializar(ListaProcesos *lista) {
    if (!lista) return;
    lista->contador = 0;
    lista->siguienteOrden = 1;
}

int listaAgregar(ListaProcesos *lista, int pid, int burstTime, int tamanoBloques) {
    if (!lista) return -1;
    if (lista->contador >= MAX_LISTA) {
        mostrarError("mkprocess", "la lista de procesos esta llena (maximo %d); elimine "
                     "procesos con kbill", MAX_LISTA);
        return -1;
    }
    if (listaBuscar(lista, pid) != NULL) {
        mostrarError("mkprocess", "ya existe un proceso con identificador %d", pid);
        return -1;
    }

    PCB *p = &lista->datos[lista->contador];
    memset(p, 0, sizeof(*p));
    p->pid = pid;
    p->burstTime = burstTime;
    p->tamanoBloques = tamanoBloques;
    p->remainingTime = burstTime;
    p->startTime = -1;
    p->finishTime = -1;
    p->estado = ESTADO_NUEVO;

    lista->contador++;
    return 0;
}

PCB *listaBuscar(ListaProcesos *lista, int pid) {
    if (!lista) return NULL;
    for (int i = 0; i < lista->contador; i++)
        if (lista->datos[i].pid == pid) return &lista->datos[i];
    return NULL;
}

int listaEliminar(ListaProcesos *lista, int pid) {
    if (!lista) return -1;
    for (int i = 0; i < lista->contador; i++) {
        if (lista->datos[i].pid != pid) continue;
        /* recorrer los siguientes un lugar para no dejar hueco */
        memmove(&lista->datos[i], &lista->datos[i + 1],
                (size_t)(lista->contador - i - 1) * sizeof(PCB));
        lista->contador--;
        return 0;
    }
    return -1;
}

void listaMarcarListo(ListaProcesos *lista, PCB *proceso) {
    proceso->estado = ESTADO_LISTO;
    proceso->ordenLlegada = lista->siguienteOrden++;
}

int listaObtenerListos(ListaProcesos *lista, PCB **listos) {
    int n = 0;
    for (int i = 0; i < lista->contador; i++)
        if (lista->datos[i].estado == ESTADO_LISTO) listos[n++] = &lista->datos[i];

    /* ordenar por orden de llegada (insercion: la lista es pequena) */
    for (int i = 1; i < n; i++) {
        PCB *actual = listos[i];
        int j = i - 1;
        while (j >= 0 && listos[j]->ordenLlegada > actual->ordenLlegada) {
            listos[j + 1] = listos[j];
            j--;
        }
        listos[j + 1] = actual;
    }
    return n;
}

int listaContarEstado(const ListaProcesos *lista, EstadoProceso estado) {
    int n = 0;
    for (int i = 0; i < lista->contador; i++)
        if (lista->datos[i].estado == estado) n++;
    return n;
}

const char *estadoTexto(EstadoProceso estado) {
    switch (estado) {
    case ESTADO_NUEVO:     return "NEW";
    case ESTADO_LISTO:     return "READY";
    case ESTADO_EJECUCION: return "RUNNING";
    case ESTADO_TERMINADO: return "TERMINATED";
    }
    return "?";
}

const char *estadoColor(EstadoProceso estado) {
    switch (estado) {
    case ESTADO_NUEVO:     return colorSalida(ANSI_CIAN);
    case ESTADO_LISTO:     return colorSalida(ANSI_VERDE);
    case ESTADO_EJECUCION: return colorSalida(ANSI_AMARILLO);
    case ESTADO_TERMINADO: return colorSalida(ANSI_TENUE);
    }
    return "";
}

void listaMostrar(const ListaProcesos *lista) {
    if (!lista || lista->contador == 0) {
        printf("No hay procesos. Cree uno con: mkprocess <id> <burst time> <bloques>\n");
        return;
    }

    imprimirTituloPrincipal("Tabla de procesos");
    printf("%s  %-8s %6s %8s   %-11s %-10s %-5s %8s %8s%s\n", colorSalida(ANSI_NEGRITA),
           "PROCESO", "BURST", "BLOQUES", "ESTADO", "MEMORIA", "ALG.", "TAT", "WT",
           colorSalida(ANSI_RESET));
    imprimirLineaSeparadora("─", ANCHO_REPORTE);

    for (int i = 0; i < lista->contador; i++) {
        const PCB *p = &lista->datos[i];
        char etiqueta[16], memoria[32] = "-", tat[16] = "-", wt[16] = "-";
        int inicio;

        snprintf(etiqueta, sizeof(etiqueta), "P%d", p->pid);
        if (memoriaBuscar(&memoriaGlobal, p->pid, &inicio))
            snprintf(memoria, sizeof(memoria), "%d-%d", inicio, inicio + p->tamanoBloques - 1);
        if (p->estado == ESTADO_TERMINADO) {   /* metricas de su ejecucion */
            snprintf(tat, sizeof(tat), "%d", p->turnaroundTime);
            snprintf(wt, sizeof(wt), "%d", p->waitingTime);
        }
        printf("  %-8s %6d %8d   %s%-11s%s %-10s %-5s %8s %8s\n", etiqueta, p->burstTime,
               p->tamanoBloques, estadoColor(p->estado), estadoTexto(p->estado),
               colorSalida(ANSI_RESET), memoria, p->algoritmo ? p->algoritmo : "-", tat, wt);
    }

    imprimirLineaSeparadora("─", ANCHO_REPORTE);
    printf("  Total: %d proceso%s  |  NEW: %d  READY: %d  TERMINATED: %d\n",
           lista->contador, lista->contador == 1 ? "" : "s",
           listaContarEstado(lista, ESTADO_NUEVO), listaContarEstado(lista, ESTADO_LISTO),
           listaContarEstado(lista, ESTADO_TERMINADO));
}
