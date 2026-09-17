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

/* Includes de comandos */
#include "commands/info.h"
#include "commands/join.h"
#include "commands/copy.h"
#include "commands/del.h"
#include "commands/cd.h"
#include "commands/sdir.h"
#include "commands/clear.h"
#include "commands/exit_cmd.h"

/* Constantes */
#define INPUT_SIZE 512
#define BUFFER_SIZE 1024

/* Colores ANSI */
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_BLUE    "\033[1;34m"
#define COLOR_WHITE   "\033[1;37m"
#define COLOR_RED     "\033[1;31m"

#endif /* MAIN_H */
