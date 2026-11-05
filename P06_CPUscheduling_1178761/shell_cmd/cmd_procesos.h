#ifndef CMD_PROCESOS_H
#define CMD_PROCESOS_H

#include "../process/lista_procesos.h"

/* Comandos oficiales de la Practica 6 (camelCase) */
void cmdMkprocess(ListaProcesos *lista, char **args);
void cmdLstprocss(ListaProcesos *lista);
void cmdFcfs(ListaProcesos *lista);
void cmdSjf(ListaProcesos *lista);
void cmdRr(ListaProcesos *lista);

/* Alias de compatibilidad con nombres anteriores */
#define cmd_mkprocess   cmdMkprocess
#define cmd_lstprocss   cmdLstprocss
#define cmd_fcfs        cmdFcfs
#define cmd_sjf         cmdSjf
#define cmd_rr          cmdRr
#define cmdCrearProc    cmdMkprocess
#define cmdMostrarProc  cmdLstprocss
#define cmdPlanFCFS     cmdFcfs
#define cmdPlanSJF      cmdSjf
#define cmdPlanRR       cmdRr

#endif /* CMD_PROCESOS_H */
