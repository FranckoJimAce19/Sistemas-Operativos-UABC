/**
 * @file  cmd_procesos.c
 * @brief Comandos de procesos y planificacion de CPU.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmd_procesos.h"
#include "../memory/memoria.h"
#include "../scheduler/fcfs.h"
#include "../scheduler/sjf.h"
#include "../scheduler/rr.h"
#include "../scheduler/sched_util.h"
#include "../utilidades/terminal.h"
#include "../utilidades/validacion.h"

#define MAX_ID    9999   /* identificador valido: 1..9999 */
#define MAX_BURST 1000   /* burst time valido: 1..1000 ut */

PCB *buscarProcesoArgumento(ListaProcesos *lista, const char *comando, const char *texto) {
    int pid;
    if (leerEntero(comando, "identificador", texto, 1, MAX_ID, &pid) != 0) return NULL;

    PCB *p = listaBuscar(lista, pid);
    if (!p) mostrarError(comando, "no existe el proceso con identificador %d "
                         "(use lstprocess para ver los procesos)", pid);
    return p;
}

void cmdMkprocess(ListaProcesos *lista, char **args) {
    if (!lista) return;
    if (!args[1] || !args[2] || !args[3] || args[4]) {
        mostrarError("mkprocess", args[4] ? "demasiados argumentos" : "faltan argumentos");
        mostrarUso("mkprocess <id> <burst time> <bloques>");
        return;
    }
    int pid, burstTime, bloques;
    if (leerEntero("mkprocess", "identificador", args[1], 1, MAX_ID, &pid) != 0 ||
        leerEntero("mkprocess", "burst time", args[2], 1, MAX_BURST, &burstTime) != 0 ||
        leerEntero("mkprocess", "tamano en bloques", args[3], 1, TOTAL_BLOQUES, &bloques) != 0) {
        mostrarUso("mkprocess <id> <burst time> <bloques>");
        return;
    }

    if (listaAgregar(lista, pid, burstTime, bloques) == 0)
        mostrarExito("Proceso P%d creado: burst time %d ut, %d bloque%s, estado %sNEW%s",
                     pid, burstTime, bloques, bloques == 1 ? "" : "s",
                     estadoColor(ESTADO_NUEVO), colorSalida(ANSI_RESET));
}

void cmdLstprocess(ListaProcesos *lista) {
    if (!lista) return;
    listaMostrar(lista);
}

void cmdKbill(ListaProcesos *lista, char **args) {
    if (!lista) return;
    if (!args[1] || args[2]) {
        mostrarError("kbill", args[1] ? "demasiados argumentos" : "faltan argumentos");
        mostrarUso("kbill <id>");
        return;
    }
    PCB *p = buscarProcesoArgumento(lista, "kbill", args[1]);
    if (!p) return;

    int pid = p->pid;
    EstadoProceso estadoAnterior = p->estado;
    /* si esta en READY ocupa memoria: se hace el free internamente */
    int liberados = estadoAnterior == ESTADO_LISTO ? memoriaLiberar(&memoriaGlobal, pid) : 0;
    listaEliminar(lista, pid);

    if (liberados > 0)
        mostrarExito("Proceso P%d eliminado (estaba en %s; se liberaron %d bloques)", pid,
                     estadoTexto(estadoAnterior), liberados);
    else
        mostrarExito("Proceso P%d eliminado (estaba en %s)", pid, estadoTexto(estadoAnterior));
}

/**
 * @brief Comun a fcfs, sjf y rr: ejecuta los procesos READY y despues los
 *        pasa a TERMINATED liberando su memoria.
 */
static void ejecutarPlanificador(ListaProcesos *lista, const char *comando, const char *nombre,
                                 void (*algoritmo)(PCB **, int)) {
    PCB *listos[MAX_LISTOS];
    int contador = listaObtenerListos(lista, listos);

    if (contador == 0) {
        if (listaContarEstado(lista, ESTADO_NUEVO) > 0)
            mostrarError(comando, "no hay procesos en READY; carguelos a memoria con "
                         "alloc <id> <first | best | worst>");
        else
            mostrarError(comando, "no hay procesos en READY; cree procesos con mkprocess "
                         "y carguelos con alloc");
        return;
    }

    algoritmo(listos, contador);

    int liberados = 0;
    for (int i = 0; i < contador; i++) {
        listos[i]->estado = ESTADO_TERMINADO;
        listos[i]->algoritmo = nombre;
        liberados += memoriaLiberar(&memoriaGlobal, listos[i]->pid);
    }
    printf("\n");
    mostrarExito("%d proceso%s pasaron a TERMINATED y liberaron %d bloques de memoria",
                 contador, contador == 1 ? "" : "s", liberados);

    /* avisar cuales procesos NEW se quedaron sin ejecutar */
    char pendientes[512] = "";
    size_t usado = 0;
    int nuevos = 0;
    for (int i = 0; i < lista->contador && usado < sizeof(pendientes) - 16; i++) {
        if (lista->datos[i].estado != ESTADO_NUEVO) continue;
        usado += (size_t)snprintf(pendientes + usado, sizeof(pendientes) - usado, "%sP%d",
                                  nuevos > 0 ? ", " : "", lista->datos[i].pid);
        nuevos++;
    }
    if (nuevos > 0)
        mostrarAviso("%s %s en NEW (sin memoria asignada) y no se ejecut%s", pendientes,
                     nuevos == 1 ? "sigue" : "siguen", nuevos == 1 ? "o" : "aron");
}

void cmdFcfs(ListaProcesos *lista) {
    if (!lista) return;
    ejecutarPlanificador(lista, "fcfs", "FCFS", ejecutarFCFS);
}

void cmdSjf(ListaProcesos *lista) {
    if (!lista) return;
    ejecutarPlanificador(lista, "sjf", "SJF", ejecutarSJF);
}

void cmdRr(ListaProcesos *lista) {
    if (!lista) return;
    ejecutarPlanificador(lista, "rr", "RR", ejecutarRR);
}
