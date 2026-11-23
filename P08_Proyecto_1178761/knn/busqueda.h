/**
 * @file  busqueda.h
 * @brief Busqueda de hiper-parametros K-NN con hilos.
 *        Cada hilo toma valores de k del espacio {1,3,5,...,17} de forma
 *        sincronizada (mutex + barrera) y calcula su tasa de reconocimiento.
 */
#ifndef BUSQUEDA_H
#define BUSQUEDA_H

#include "dataset.h"

#define NUM_K     9   /* tamano del espacio de busqueda */
#define MAX_HILOS 3   /* se comparan corridas con 1, 2 y 3 hilos */

/** Espacio de busqueda de k (valores impares de 1 a 17). */
extern const int K_VALORES[NUM_K];

/** Variable global: cantidad de hilos de la siguiente corrida (1..MAX_HILOS). */
extern int numHilosBusqueda;

/** Resultado de la busqueda para un valor de k. */
typedef struct {
    int k;
    double tasa;        /* tasa de reconocimiento en [0.0, 1.0] */
} ResultadoK;

/** Reporte completo de una corrida de la busqueda. */
typedef struct {
    ResultadoK resultados[NUM_K];
    int completados;    /* cantidad de k evaluados */
    double tiempoSeg;   /* tiempo de la corrida (solo la busqueda) */
    int mejorK;         /* k con mayor tasa de reconocimiento */
    double mejorTasa;
} ReporteBusqueda;

/**
 * @brief Ejecuta la busqueda con numHilosBusqueda hilos y mide su tiempo.
 * @return 0 si ok, -1 si algun argumento es NULL.
 */
int busquedaEjecutar(const Dataset *train, const Dataset *test, ReporteBusqueda *rep);

/**
 * @brief Busqueda completa: carga el dataset de 'dir' y
 *        ejecuta la busqueda con 'hilos' hilos. Si hilos == 0 hace las
 *        corridas con 1, 2 y 3 hilos e imprime la comparacion de tiempos.
 * @return 0 si ok, -1 en error.
 */
int busquedaKnn(const char *dir, int hilos);

#endif /* BUSQUEDA_H */
