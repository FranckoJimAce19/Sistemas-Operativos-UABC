/**
 * @file  rr.h
 * @brief Simulacion Round-Robin con quantum QUANTUM_RR.
 */
#ifndef RR_H
#define RR_H

#include "../process/lista_procesos.h"

#define QUANTUM_RR 10

void ejecutarRR(ListaProcesos *listaOriginal);

#endif /* RR_H */
