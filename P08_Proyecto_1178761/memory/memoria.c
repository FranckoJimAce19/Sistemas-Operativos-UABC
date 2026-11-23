/**
 * @file  memoria.c
 * @brief Memoria principal simulada con asignacion contigua y compactacion.
 */
#include <stdio.h>
#include <strings.h>
#include "memoria.h"
#include "../utilidades/terminal.h"

Memoria memoriaGlobal;

void memoriaInicializar(Memoria *mem) {
    for (int i = 0; i < TOTAL_BLOQUES; i++)
        mem->bloques[i] = BLOQUE_LIBRE;
}

int memoriaEstrategia(const char *texto, Estrategia *estrategia) {
    if (!texto) return -1;
    if (!strcasecmp(texto, "first") || !strcasecmp(texto, "ff") || !strcasecmp(texto, "firstfit"))
        *estrategia = FIRST_FIT;
    else if (!strcasecmp(texto, "best") || !strcasecmp(texto, "bf") || !strcasecmp(texto, "bestfit"))
        *estrategia = BEST_FIT;
    else if (!strcasecmp(texto, "worst") || !strcasecmp(texto, "wf") || !strcasecmp(texto, "worstfit"))
        *estrategia = WORST_FIT;
    else
        return -1;
    return 0;
}

const char *estrategiaTexto(Estrategia estrategia) {
    switch (estrategia) {
    case FIRST_FIT: return "first fit";
    case BEST_FIT:  return "best fit";
    case WORST_FIT: return "worst fit";
    }
    return "?";
}

int memoriaAsignar(Memoria *mem, int pid, int tamano, Estrategia estrategia, int *inicio) {
    int mejorInicio = -1, mejorTamano = 0, libres = 0;

    /* Recorrer los huecos (rangos contiguos de bloques libres) */
    for (int i = 0; i < TOTAL_BLOQUES; ) {
        if (mem->bloques[i] != BLOQUE_LIBRE) { i++; continue; }
        int inicioHueco = i;
        while (i < TOTAL_BLOQUES && mem->bloques[i] == BLOQUE_LIBRE) i++;
        int tamanoHueco = i - inicioHueco;
        libres += tamanoHueco;

        if (tamanoHueco < tamano) continue;
        int esMejor = mejorInicio == -1 ||
                      (estrategia == BEST_FIT && tamanoHueco < mejorTamano) ||
                      (estrategia == WORST_FIT && tamanoHueco > mejorTamano);
        if (esMejor) {
            mejorInicio = inicioHueco;
            mejorTamano = tamanoHueco;
            if (estrategia == FIRST_FIT) break;   /* el primero que alcanza */
        }
    }

    if (mejorInicio == -1)
        return libres >= tamano ? -2 : -1;

    for (int i = mejorInicio; i < mejorInicio + tamano; i++)
        mem->bloques[i] = pid;
    *inicio = mejorInicio;
    return 0;
}

int memoriaLiberar(Memoria *mem, int pid) {
    int liberados = 0;
    for (int i = 0; i < TOTAL_BLOQUES; i++) {
        if (mem->bloques[i] == pid) {
            mem->bloques[i] = BLOQUE_LIBRE;
            liberados++;
        }
    }
    return liberados;
}

int memoriaBuscar(const Memoria *mem, int pid, int *inicio) {
    for (int i = 0; i < TOTAL_BLOQUES; i++) {
        if (mem->bloques[i] == pid) {
            *inicio = i;
            return 1;
        }
    }
    return 0;
}

int memoriaLibres(const Memoria *mem) {
    int libres = 0;
    for (int i = 0; i < TOTAL_BLOQUES; i++)
        if (mem->bloques[i] == BLOQUE_LIBRE) libres++;
    return libres;
}

int memoriaFragmentada(const Memoria *mem) {
    /* hay fragmentacion si algun bloque ocupado esta despues de uno libre */
    int vioLibre = 0;
    for (int i = 0; i < TOTAL_BLOQUES; i++) {
        if (mem->bloques[i] == BLOQUE_LIBRE) vioLibre = 1;
        else if (vioLibre) return 1;
    }
    return 0;
}

int memoriaCompactar(Memoria *mem) {
    int destino = 0, movidos = 0, anterior = BLOQUE_LIBRE;

    for (int i = 0; i < TOTAL_BLOQUES; i++) {
        int pid = mem->bloques[i];
        if (pid == BLOQUE_LIBRE) continue;
        if (pid != anterior && i != destino) movidos++;   /* inicio de un proceso que se mueve */
        anterior = pid;
        mem->bloques[destino++] = pid;
    }
    for (int i = destino; i < TOTAL_BLOQUES; i++)
        mem->bloques[i] = BLOQUE_LIBRE;
    return movidos;
}

/** Colores con los que se distinguen los procesos en el mapa. */
static const char *const COLORES_PROCESOS[] = {
    ANSI_VERDE, ANSI_AZUL, ANSI_MAGENTA, ANSI_AMARILLO, ANSI_CIAN, ANSI_ROJO,
};

const char *memoriaColorProceso(int pid) {
    return colorSalida(COLORES_PROCESOS[pid % 6]);
}

void memoriaMostrar(const Memoria *mem) {
    char titulo[96];
    snprintf(titulo, sizeof(titulo), "Memoria principal  |  %d bloques  |  asignacion contigua",
             TOTAL_BLOQUES);
    imprimirTituloPrincipal(titulo);

    /* Mapa: cada bloque muestra el proceso que lo ocupa o '·' si esta libre */
    imprimirSubtitulo("Mapa de bloques");
    for (int fila = 0; fila < TOTAL_BLOQUES; fila += BLOQUES_FILA) {
        printf("  %s%4d%s │", colorSalida(ANSI_TENUE), fila, colorSalida(ANSI_RESET));
        for (int i = fila; i < fila + BLOQUES_FILA && i < TOTAL_BLOQUES; i++) {
            int pid = mem->bloques[i];
            if (pid == BLOQUE_LIBRE) {
                printf("%s  ·   %s", colorSalida(ANSI_TENUE), colorSalida(ANSI_RESET));
            } else {
                char etiqueta[16];
                snprintf(etiqueta, sizeof(etiqueta), "P%d", pid);
                printf(" %s%-5s%s", memoriaColorProceso(pid), etiqueta, colorSalida(ANSI_RESET));
            }
        }
        printf("\n");
    }

    /* Segmentos: procesos y huecos en orden de direccion */
    imprimirSubtitulo("Segmentos");
    printf("%s  %8s %8s %9s   %s%s\n", colorSalida(ANSI_NEGRITA), "INICIO", "FIN", "BLOQUES",
           "CONTENIDO", colorSalida(ANSI_RESET));
    int huecos = 0, mayorHueco = 0, libres = 0, procesos = 0;
    for (int i = 0; i < TOTAL_BLOQUES; ) {
        int pid = mem->bloques[i], inicio = i;
        while (i < TOTAL_BLOQUES && mem->bloques[i] == pid) i++;
        int tamano = i - inicio;
        printf("  %8d %8d %9d   ", inicio, i - 1, tamano);
        if (pid == BLOQUE_LIBRE) {
            printf("%shueco libre%s\n", colorSalida(ANSI_TENUE), colorSalida(ANSI_RESET));
            huecos++;
            libres += tamano;
            if (tamano > mayorHueco) mayorHueco = tamano;
        } else {
            printf("%sP%d%s\n", memoriaColorProceso(pid), pid, colorSalida(ANSI_RESET));
            procesos++;
        }
    }

    imprimirSubtitulo("Resumen");
    printf("  Ocupados: %d (%.1f %%)   Libres: %d   Procesos cargados: %d\n",
           TOTAL_BLOQUES - libres, 100.0 * (TOTAL_BLOQUES - libres) / TOTAL_BLOQUES,
           libres, procesos);
    printf("  Huecos libres: %d   Hueco mas grande: %d bloques\n", huecos, mayorHueco);
    if (memoriaFragmentada(mem))
        printf("  %sFragmentacion externa: %d bloques libres repartidos en %d huecos "
               "(use compact)%s\n", colorSalida(ANSI_AMARILLO), libres, huecos,
               colorSalida(ANSI_RESET));
    else
        printf("  Sin fragmentacion externa.\n");
}
