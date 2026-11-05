#ifndef PCB_H
#define PCB_H

#define MAX_PROC_NAME 32

typedef enum {
    ESTADO_NUEVO,
    ESTADO_LISTO,
    ESTADO_EJECUCION,
    ESTADO_TERMINADO
} EstadoProceso;

/**
 * @brief Bloque de Control de Proceso (PCB).
 * Metricas de SO en ingles (camelCase), atributos en espanol.
 */
typedef struct {
    int pid;                    /* Identificador unico */
    char nombre[MAX_PROC_NAME]; /* Nombre del proceso */
    int burstTime;              /* Rafaga de CPU requerida */
    int remainingTime;          /* Rafaga restante (para RR) */
    int arrivalTime;            /* Tiempo de llegada a la cola (0 por defecto) */
    int startTime;              /* Instante de primer acceso a CPU */
    int finishTime;             /* Instante de finalizacion */
    int waitingTime;            /* Turnaround - Burst */
    int turnaroundTime;         /* Finish - Arrival */
    EstadoProceso estado;       /* Estado actual */
} PCB;

#endif /* PCB_H */
