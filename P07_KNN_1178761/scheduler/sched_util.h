/**
 * @file  sched_util.h
 * @brief Utilidades comunes de los planificadores (copia, trazas, metricas).
 */
#ifndef SCHED_UTIL_H
#define SCHED_UTIL_H

#include "../process/lista_procesos.h"

void copiarLista(const ListaProcesos *origen, ListaProcesos *destino);
void calcularMetricas(PCB *datos, int contador);
void mostrarResultados(PCB *datos, int contador);
void trazaEntrada(int tiempoActual, const char *nombre, int burstORestante, int quantum);
void trazaSalida(int tiempoActual, const char *nombre, const char *detalle);

#endif /* SCHED_UTIL_H */
