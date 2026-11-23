/**
 * @file  fcfs.h
 * @brief Simulacion FCFS (First-Come First-Served, no expropiativo).
 */
#ifndef FCFS_H
#define FCFS_H

#include "../process/pcb.h"

/** @brief Simula los procesos READY (en orden de llegada) con FCFS. */
void ejecutarFCFS(PCB **listos, int contador);

#endif /* FCFS_H */
