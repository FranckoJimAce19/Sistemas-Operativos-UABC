/**
 * @file  cmd_memoria.h
 * @brief Comandos de memoria: alloc, free, mstatus y compact.
 */
#ifndef CMD_MEMORIA_H
#define CMD_MEMORIA_H

#include "../process/lista_procesos.h"

void cmdAlloc(ListaProcesos *lista, char **args);
void cmdFree(ListaProcesos *lista, char **args);
void cmdMstatus(void);
void cmdCompact(ListaProcesos *lista);

#endif /* CMD_MEMORIA_H */
