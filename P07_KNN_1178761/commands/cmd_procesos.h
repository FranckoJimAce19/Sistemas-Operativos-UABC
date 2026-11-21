/**
 * @file  cmd_procesos.h
 * @brief Comandos de planificacion de CPU — Practica 6 SO.
 */
#ifndef CMD_PROCESOS_H
#define CMD_PROCESOS_H

#include "../process/lista_procesos.h"

/* Comandos del interprete: mkprocess, lstprocss, fcfs, sjf, rr */
void cmdMkprocess(ListaProcesos *lista, char **args);
void cmdLstprocss(ListaProcesos *lista);
void cmdFcfs(ListaProcesos *lista);
void cmdSjf(ListaProcesos *lista);
void cmdRr(ListaProcesos *lista);

#endif /* CMD_PROCESOS_H */
