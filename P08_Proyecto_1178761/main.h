/**
 * @file  main.h
 * @brief Cabecera general del interprete — includes y prototipos compartidos.
 */
#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

/* Comandos basicos */
#include "commands/info.h"
#include "commands/join.h"
#include "commands/copy.h"
#include "commands/del.h"
#include "commands/cd.h"
#include "commands/sdir.h"
#include "commands/clear.h"
#include "commands/parser.h"
#include "commands/pipeline.h"

/* Procesos, planificacion de CPU y memoria */
#include "process/lista_procesos.h"
#include "memory/memoria.h"
#include "commands/cmd_procesos.h"
#include "commands/cmd_memoria.h"

/* K-NN con hilos */
#include "commands/cmd_knn.h"

/* Utilidades: editor de linea, historial y autocompletado */
#include "utilidades/editor_linea.h"
#include "utilidades/historial.h"
#include "utilidades/terminal.h"

#define INPUT_SIZE 2048   /* longitud maxima de una linea de comando */

/* Colores ANSI */
#define COLOR_RESET  "\033[0m"
#define COLOR_GREEN  "\033[1;32m"
#define COLOR_BLUE   "\033[1;34m"
#define COLOR_WHITE  "\033[1;37m"
#define COLOR_RED    "\033[1;31m"

/** @brief 1 si cmd es un comando interno del interprete. */
int isBuiltin(const char *cmd);

/** @brief Ejecuta un comando interno. @return 1 si es 'exit', 0 si no. */
int runBuiltin(char **args);

/** @brief 1 si el comando cambia el estado del interprete (no va en tuberias). */
int builtinSoloPrincipal(const char *cmd);

/** @brief Nombre del comando interno i (para TAB), "" si es alias, NULL al terminar. */
const char *nombreBuiltin(int i);

extern ListaProcesos listaProcesosGlobal;

#endif /* MAIN_H */
