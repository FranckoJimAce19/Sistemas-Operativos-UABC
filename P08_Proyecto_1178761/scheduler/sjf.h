/**
 * @file  sjf.h
 * @brief Simulacion SJF (Shortest Job First, no expropiativo).
 */
#ifndef SJF_H
#define SJF_H

#include "../process/pcb.h"

/** @brief Simula los procesos READY (en orden de llegada) con SJF. */
void ejecutarSJF(PCB **listos, int contador);

#endif /* SJF_H */
