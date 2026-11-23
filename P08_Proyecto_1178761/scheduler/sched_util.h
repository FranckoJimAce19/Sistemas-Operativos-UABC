/**
 * @file  sched_util.h
 * @brief Utilidades comunes de los planificadores (copia, trazas, Gantt y
 *        metricas).
 */
#ifndef SCHED_UTIL_H
#define SCHED_UTIL_H

#include "../process/pcb.h"
#include "../utilidades/terminal.h"

#define MAX_LISTOS 64   /* procesos maximos en la ready queue */

/** @brief Copia los procesos READY a 'copia' y reinicia sus tiempos. */
void copiarListos(PCB **listos, int contador, PCB *copia);

/** @brief Guarda finish, waiting y turnaround de la simulacion en los PCB reales. */
void guardarResultados(PCB **listos, const PCB *copia, int contador);

/**
 * @brief Imprime el titulo y la ready queue, y reinicia el diagrama de Gantt.
 * @param quantum Time quantum (0 si el algoritmo no usa quantum).
 */
void iniciarSimulacion(const char *nombre, const char *descripcion, int quantum,
                       const PCB *datos, int contador);

/** @brief Traza: el proceso entra al CPU en tiempoActual. */
void trazaEntrada(int tiempoActual, int pid, int burstORestante, int quantum);

/** @brief Traza: el proceso sale del CPU; restante = 0 significa que termino. */
void trazaSalida(int tiempoActual, int pid, int restante);

void calcularMetricas(PCB *datos, int contador);

/** @brief Imprime el diagrama de Gantt, las metricas por proceso y los promedios. */
void mostrarResultados(PCB *datos, int contador);

#endif /* SCHED_UTIL_H */
