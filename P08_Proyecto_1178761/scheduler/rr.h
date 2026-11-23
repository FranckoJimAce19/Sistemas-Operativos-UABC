/**
 * @file  rr.h
 * @brief Simulacion Round-Robin con quantum QUANTUM_RR.
 */
#ifndef RR_H
#define RR_H

#include "../process/pcb.h"

#define QUANTUM_RR 10

/** @brief Simula los procesos READY (en orden de llegada) con RR. */
void ejecutarRR(PCB **listos, int contador);

#endif /* RR_H */
