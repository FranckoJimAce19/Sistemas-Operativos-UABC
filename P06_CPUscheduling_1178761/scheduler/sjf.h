#ifndef SJF_H
#define SJF_H

#include "../process/lista_procesos.h"

void ejecutarSJF(ListaProcesos *listaOriginal);

/* Alias de compatibilidad */
#define ejecutar_sjf ejecutarSJF

#endif /* SJF_H */
