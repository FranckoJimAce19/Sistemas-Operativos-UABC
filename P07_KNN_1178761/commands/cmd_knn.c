/**
 * @file  cmd_knn.c
 * @brief Comando 'knn' del interprete — Practica 7 SO.
 *        Reutiliza el modulo knn/ (el mismo que usa ./busqueda_knn).
 */
#include <stdio.h>
#include <stdlib.h>
#include "cmd_knn.h"
#include "../knn/busqueda.h"

void cmdKnn(char **args) {
    const char *dir = args[1] ? args[1] : "data";
    int hilos = 0;                  /* 0 = comparar 1, 2 y 3 hilos */

    if (args[1] && args[2]) {
        hilos = atoi(args[2]);
        if (hilos < 1 || hilos > MAX_HILOS) {
            printf("Uso: knn [directorio] [1-%d]\n", MAX_HILOS);
            printf("Ejemplo: knn data 3\n");
            return;
        }
    }
    busquedaPractica(dir, hilos);
}
