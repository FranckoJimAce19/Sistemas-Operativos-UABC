/**
 * @file  pcb.h
 * @brief Bloque de Control de Proceso (PCB) de los procesos simulados.
 *
 * Ciclo de vida:  mkprocess -> NEW --alloc--> READY --fcfs/sjf/rr--> TERMINATED
 *                 free regresa un proceso READY a NEW; kbill lo elimina.
 */
#ifndef PCB_H
#define PCB_H

typedef enum {
    ESTADO_NUEVO,       /* NEW: creado, fuera de memoria */
    ESTADO_LISTO,       /* READY: cargado en memoria (ready queue) */
    ESTADO_EJECUCION,   /* RUNNING: usando el CPU durante la simulacion */
    ESTADO_TERMINADO    /* TERMINATED: ya se ejecuto, no puede volver a correr */
} EstadoProceso;

/**
 * @brief Bloque de Control de Proceso (PCB).
 * Metricas de SO en ingles (camelCase), atributos en espanol.
 */
typedef struct {
    int pid;                    /* Identificador dado en mkprocess */
    int burstTime;              /* Rafaga de CPU requerida */
    int tamanoBloques;          /* Bloques de memoria que ocupa */
    int remainingTime;          /* Rafaga restante (para RR) */
    int arrivalTime;            /* Tiempo de llegada a la cola (0 por defecto) */
    int startTime;              /* Instante de primer acceso a CPU */
    int finishTime;             /* Instante de finalizacion */
    int waitingTime;            /* Turnaround - Burst */
    int turnaroundTime;         /* Finish - Arrival */
    int ordenLlegada;           /* Orden en que entro a la ready queue (alloc) */
    const char *algoritmo;      /* Algoritmo con el que se ejecuto (o NULL) */
    EstadoProceso estado;       /* Estado actual */
} PCB;

#endif /* PCB_H */
