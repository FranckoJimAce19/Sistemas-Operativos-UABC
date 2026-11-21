/**
 * @file  main_knn.c
 * @brief Programa independiente de la Practica 7: busqueda de hiper-parametros
 *        K-NN con 1, 2 y 3 hilos.
 *
 *  Uso: ./busqueda_knn [directorio] [hilos]
 *       directorio: donde estan los 4 CSV (por defecto: data)
 *       hilos:      1..3; si se omite se comparan las tres corridas
 */
#include <stdio.h>
#include <stdlib.h>
#include "busqueda.h"

int main(int argc, char **argv) {
    const char *dir = argc > 1 ? argv[1] : "data";
    int hilos = 0;

    if (argc > 2) {
        hilos = atoi(argv[2]);
        if (hilos < 1 || hilos > MAX_HILOS) {
            fprintf(stderr, "Uso: %s [directorio] [1-%d]\n", argv[0], MAX_HILOS);
            return 1;
        }
    }
    return busquedaPractica(dir, hilos) == 0 ? 0 : 1;
}
