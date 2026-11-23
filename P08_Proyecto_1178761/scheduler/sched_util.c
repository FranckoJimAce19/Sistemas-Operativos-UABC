/**
 * @file  sched_util.c
 * @brief Utilidades comunes de los planificadores (copia, trazas, Gantt y
 *        metricas).
 */
#include <stdio.h>
#include <string.h>
#include "sched_util.h"

#define MAX_SEGMENTOS 8192   /* intervalos de CPU guardados para el Gantt */
#define ANCHO_GANTT   200    /* ancho maximo de un renglon del Gantt */

/** Intervalo en que un proceso uso el CPU (para el diagrama de Gantt). */
typedef struct {
    int pid;
    int inicio;
    int fin;
} Segmento;

static Segmento segmentos[MAX_SEGMENTOS];
static int numSegmentos = 0;
static int inicioActual = 0;    /* instante de la ultima entrada al CPU */

void copiarListos(PCB **listos, int contador, PCB *copia) {
    for (int i = 0; i < contador; i++) {
        copia[i] = *listos[i];
        copia[i].remainingTime = copia[i].burstTime;
        copia[i].arrivalTime = 0;            /* todos estan en la ready queue en t = 0 */
        copia[i].startTime = -1;
        copia[i].finishTime = -1;
        copia[i].waitingTime = 0;
        copia[i].turnaroundTime = 0;
        copia[i].estado = ESTADO_LISTO;
    }
}

void guardarResultados(PCB **listos, const PCB *copia, int contador) {
    for (int i = 0; i < contador; i++) {
        listos[i]->startTime = copia[i].startTime;
        listos[i]->finishTime = copia[i].finishTime;
        listos[i]->waitingTime = copia[i].waitingTime;
        listos[i]->turnaroundTime = copia[i].turnaroundTime;
    }
}

void iniciarSimulacion(const char *nombre, const char *descripcion, int quantum,
                       const PCB *datos, int contador) {
    char titulo[128];
    if (quantum > 0)
        snprintf(titulo, sizeof(titulo), "Planificacion %s  |  %s  |  quantum = %d ut",
                 nombre, descripcion, quantum);
    else
        snprintf(titulo, sizeof(titulo), "Planificacion %s  |  %s", nombre, descripcion);
    imprimirTituloPrincipal(titulo);

    printf("Ready queue (orden de llegada): ");
    for (int i = 0; i < contador; i++)
        printf("%sP%d", i > 0 ? " -> " : "", datos[i].pid);
    printf("\n");

    numSegmentos = 0;
    imprimirSubtitulo("Orden de entrada y salida del CPU");
}

void trazaEntrada(int tiempoActual, int pid, int burstORestante, int quantum) {
    (void)burstORestante;
    (void)quantum;
    inicioActual = tiempoActual;
    printf("  t = %5d   %sentra%s  P%d\n", tiempoActual,
           colorSalida(ANSI_VERDE), colorSalida(ANSI_RESET), pid);
}

void trazaSalida(int tiempoActual, int pid, int restante) {
    char etiqueta[16];
    snprintf(etiqueta, sizeof(etiqueta), "P%d", pid);

    printf("  t = %5d   %ssale %s  %-6s ", tiempoActual,
           colorSalida(ANSI_AMARILLO), colorSalida(ANSI_RESET), etiqueta);
    if (restante == 0)
        printf("(termina)\n");
    else
        printf("(quantum agotado, restan %d ut)\n", restante);

    if (numSegmentos < MAX_SEGMENTOS) {
        segmentos[numSegmentos].pid = pid;
        segmentos[numSegmentos].inicio = inicioActual;
        segmentos[numSegmentos].fin = tiempoActual;
        numSegmentos++;
    }
}

void calcularMetricas(PCB *datos, int contador) {
    if (!datos || contador <= 0) return;
    for (int i = 0; i < contador; i++) {
        datos[i].turnaroundTime = datos[i].finishTime - datos[i].arrivalTime;
        datos[i].waitingTime = datos[i].turnaroundTime - datos[i].burstTime;
        if (datos[i].waitingTime < 0) datos[i].waitingTime = 0;
    }
}

/**
 * @brief Imprime el diagrama de Gantt; si no cabe en la terminal lo parte
 *        en varios renglones.
 */
static void mostrarGantt(void) {
    imprimirSubtitulo("Diagrama de Gantt");

    int anchoDisponible = obtenerAnchoTerminal() - 10;
    if (anchoDisponible > ANCHO_GANTT) anchoDisponible = ANCHO_GANTT;
    if (anchoDisponible < 20) anchoDisponible = 20;

    char barras[ANCHO_GANTT + 64] = "|";
    char tiempos[ANCHO_GANTT + 64] = "";
    int ancho = 1;

    for (int i = 0; i < numSegmentos; i++) {
        char etiqueta[16], inicio[16];
        snprintf(etiqueta, sizeof(etiqueta), "P%d", segmentos[i].pid);
        snprintf(inicio, sizeof(inicio), "%d", segmentos[i].inicio);

        /* la celda debe caber la etiqueta (con un espacio a cada lado) y el tiempo */
        int anchoCelda = (int)strlen(etiqueta) + 2;
        if ((int)strlen(inicio) + 1 > anchoCelda) anchoCelda = (int)strlen(inicio) + 1;

        if (ancho + anchoCelda + 1 > anchoDisponible && ancho > 1) {   /* nuevo renglon */
            printf("  %s\n  %s%d\n", barras, tiempos, segmentos[i].inicio);
            strcpy(barras, "|");
            tiempos[0] = '\0';
            ancho = 1;
        }

        int izquierda = (anchoCelda - (int)strlen(etiqueta)) / 2;
        int derecha = anchoCelda - (int)strlen(etiqueta) - izquierda;
        size_t largo = strlen(barras);
        snprintf(barras + largo, sizeof(barras) - largo, "%*s%s%*s|",
                 izquierda, "", etiqueta, derecha, "");
        largo = strlen(tiempos);
        snprintf(tiempos + largo, sizeof(tiempos) - largo, "%-*s", anchoCelda + 1, inicio);
        ancho += anchoCelda + 1;
    }
    int fin = numSegmentos > 0 ? segmentos[numSegmentos - 1].fin : 0;
    printf("  %s\n  %s%d\n", barras, tiempos, fin);
}

void mostrarResultados(PCB *datos, int contador) {
    if (!datos || contador <= 0) {
        printf("No hay procesos para mostrar resultados.\n");
        return;
    }
    calcularMetricas(datos, contador);
    mostrarGantt();

    imprimirSubtitulo("Resultados por proceso");
    printf("%s  %-8s %10s %14s %14s %14s%s\n", colorSalida(ANSI_NEGRITA),
           "PROCESO", "BURST", "FINALIZACION", "TURNAROUND", "WAITING", colorSalida(ANSI_RESET));

    float sumaWaiting = 0.0f;
    float sumaTurnaround = 0.0f;
    for (int i = 0; i < contador; i++) {
        char etiqueta[16];
        snprintf(etiqueta, sizeof(etiqueta), "P%d", datos[i].pid);
        printf("  %-8s %10d %14d %14d %14d\n", etiqueta, datos[i].burstTime,
               datos[i].finishTime, datos[i].turnaroundTime, datos[i].waitingTime);
        sumaWaiting += (float)datos[i].waitingTime;
        sumaTurnaround += (float)datos[i].turnaroundTime;
    }
    imprimirLineaSeparadora("─", ANCHO_REPORTE);
    printf("  %-34s %14.2f %14.2f\n", "Promedio (ut)",
           sumaTurnaround / (float)contador, sumaWaiting / (float)contador);
}
