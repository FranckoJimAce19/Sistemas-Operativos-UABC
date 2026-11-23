/**
 * @file  cmd_procesos.h
 * @brief Comandos de procesos y planificacion de CPU.
 */
#ifndef CMD_PROCESOS_H
#define CMD_PROCESOS_H

#include "../process/lista_procesos.h"

/* Comandos del interprete: mkprocess, lstprocess, kbill, fcfs, sjf, rr */
void cmdMkprocess(ListaProcesos *lista, char **args);
void cmdLstprocess(ListaProcesos *lista);
void cmdKbill(ListaProcesos *lista, char **args);
void cmdFcfs(ListaProcesos *lista);
void cmdSjf(ListaProcesos *lista);
void cmdRr(ListaProcesos *lista);

/**
 * @brief Convierte el texto en identificador y busca el proceso.
 *        Imprime el error si no es valido o no existe.
 * @return El proceso, o NULL.
 */
PCB *buscarProcesoArgumento(ListaProcesos *lista, const char *comando, const char *texto);

#endif /* CMD_PROCESOS_H */
