/**
 * @file  cmd_memoria.c
 * @brief Comandos de memoria: alloc, free, mstatus y compact.
 */
#include <stdio.h>
#include "cmd_memoria.h"
#include "cmd_procesos.h"
#include "../memory/memoria.h"
#include "../utilidades/terminal.h"

void cmdAlloc(ListaProcesos *lista, char **args) {
    if (!lista) return;
    if (!args[1] || !args[2] || args[3]) {
        mostrarError("alloc", args[1] && args[2] ? "demasiados argumentos" : "faltan argumentos");
        mostrarUso("alloc <id> <first | best | worst>");
        return;
    }
    PCB *p = buscarProcesoArgumento(lista, "alloc", args[1]);
    if (!p) return;

    Estrategia estrategia;
    if (memoriaEstrategia(args[2], &estrategia) != 0) {
        mostrarError("alloc", "estrategia '%s' no valida (use first, best o worst)", args[2]);
        return;
    }
    if (p->estado == ESTADO_TERMINADO) {
        mostrarError("alloc", "P%d ya termino su ejecucion y no puede volver a cargarse", p->pid);
        return;
    }
    if (p->estado == ESTADO_LISTO) {
        int inicio = 0;
        memoriaBuscar(&memoriaGlobal, p->pid, &inicio);
        mostrarError("alloc", "P%d ya esta en memoria (bloques %d-%d)", p->pid, inicio,
                     inicio + p->tamanoBloques - 1);
        return;
    }

    int inicio;
    int resultado = memoriaAsignar(&memoriaGlobal, p->pid, p->tamanoBloques, estrategia, &inicio);
    if (resultado == -2) {
        mostrarError("alloc", "P%d requiere %d bloques contiguos; hay %d libres pero no "
                     "contiguos (fragmentacion externa). Use compact.",
                     p->pid, p->tamanoBloques, memoriaLibres(&memoriaGlobal));
        return;
    }
    if (resultado != 0) {
        mostrarError("alloc", "memoria insuficiente: P%d requiere %d bloques y solo hay %d libres",
                     p->pid, p->tamanoBloques, memoriaLibres(&memoriaGlobal));
        return;
    }
    listaMarcarListo(lista, p);
    mostrarExito("P%d cargado en los bloques %d-%d con %s; estado %sREADY%s", p->pid, inicio,
                 inicio + p->tamanoBloques - 1, estrategiaTexto(estrategia),
                 estadoColor(ESTADO_LISTO), colorSalida(ANSI_RESET));
}

void cmdFree(ListaProcesos *lista, char **args) {
    if (!lista) return;
    if (!args[1] || args[2]) {
        mostrarError("free", args[1] ? "demasiados argumentos" : "faltan argumentos");
        mostrarUso("free <id>");
        return;
    }
    PCB *p = buscarProcesoArgumento(lista, "free", args[1]);
    if (!p) return;

    if (p->estado == ESTADO_NUEVO) {
        mostrarError("free", "P%d no esta en memoria (estado NEW)", p->pid);
        return;
    }
    if (p->estado == ESTADO_TERMINADO) {
        mostrarError("free", "P%d ya termino y no ocupa memoria", p->pid);
        return;
    }
    int liberados = memoriaLiberar(&memoriaGlobal, p->pid);
    p->estado = ESTADO_NUEVO;   /* sale de la ready queue y puede volver a cargarse */
    mostrarExito("P%d salio de memoria (%d bloques liberados); estado %sNEW%s", p->pid,
                 liberados, estadoColor(ESTADO_NUEVO), colorSalida(ANSI_RESET));
}

void cmdMstatus(void) {
    memoriaMostrar(&memoriaGlobal);
}

void cmdCompact(ListaProcesos *lista) {
    if (!memoriaFragmentada(&memoriaGlobal)) {
        printf("La memoria ya esta compactada: no hay huecos entre procesos.\n");
        return;
    }

    /* posicion de cada proceso antes de compactar, para mostrar a donde se movio */
    int pidAntes[TOTAL_BLOQUES], inicioAntes[TOTAL_BLOQUES], cantidad = 0;
    for (int i = 0; i < TOTAL_BLOQUES; i++) {
        int pid = memoriaGlobal.bloques[i];
        if (pid != BLOQUE_LIBRE && (i == 0 || memoriaGlobal.bloques[i - 1] != pid)) {
            pidAntes[cantidad] = pid;
            inicioAntes[cantidad++] = i;
        }
    }

    int movidos = memoriaCompactar(&memoriaGlobal);

    printf("Procesos reubicados:\n");
    for (int i = 0; i < cantidad; i++) {
        int inicioNuevo;
        if (!memoriaBuscar(&memoriaGlobal, pidAntes[i], &inicioNuevo) ||
            inicioNuevo == inicioAntes[i])
            continue;
        PCB *p = listaBuscar(lista, pidAntes[i]);
        int tamano = p ? p->tamanoBloques : 1;
        printf("  %sP%-5d%s %4d-%-4d ->  %4d-%d\n", memoriaColorProceso(pidAntes[i]), pidAntes[i],
               colorSalida(ANSI_RESET), inicioAntes[i], inicioAntes[i] + tamano - 1,
               inicioNuevo, inicioNuevo + tamano - 1);
    }

    int libres = memoriaLibres(&memoriaGlobal);
    mostrarExito("Memoria compactada: %d proceso%s reubicado%s; hueco libre unico de %d bloques "
                 "(%d-%d)", movidos, movidos == 1 ? "" : "s", movidos == 1 ? "" : "s", libres,
                 TOTAL_BLOQUES - libres, TOTAL_BLOQUES - 1);
}
