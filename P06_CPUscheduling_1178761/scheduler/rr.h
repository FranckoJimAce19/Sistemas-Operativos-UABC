#ifndef RR_H
#define RR_H

#include "../process/lista_procesos.h"

#define QUANTUM_RR 10

void ejecutarRR(ListaProcesos *listaOriginal);

/* Alias de compatibilidad */
#define ejecutar_rr ejecutarRR

#endif /* RR_H */
