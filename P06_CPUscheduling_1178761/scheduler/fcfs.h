#ifndef FCFS_H
#define FCFS_H

#include "../process/lista_procesos.h"

void ejecutarFCFS(ListaProcesos *listaOriginal);

/* Alias de compatibilidad */
#define ejecutar_fcfs ejecutarFCFS

#endif /* FCFS_H */
