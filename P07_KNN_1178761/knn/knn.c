/**
 * @file  knn.c
 * @brief Clasificador K-NN: distancia euclidiana al cuadrado y votacion
 *        por mayoria. No usa sqrt: para elegir los k vecinos basta comparar
 *        distancias al cuadrado (el orden es el mismo).
 */
#include <stdlib.h>
#include "knn.h"

/**
 * @brief Distancia euclidiana al cuadrado entre dos vectores de n elementos.
 */
static double dist2(const double *a, const double *b, int n) {
    double suma = 0.0;
    for (int i = 0; i < n; i++) {
        double d = a[i] - b[i];
        suma += d * d;
    }
    return suma;
}

/**
 * @brief Inserta (d, id) en la ventana ordenada mejores[0..k-1] si es mas
 *        cercano que el peor de ellos. 'llenos' es cuantos hay hasta ahora.
 */
static void insertarVecino(double *mejorDist, int *mejorIdx, int k, int llenos,
                           double d, int id) {
    int j;
    if (llenos < k) {
        j = llenos;                                 /* aun hay espacio */
    } else {
        if (d >= mejorDist[k - 1]) return;          /* no entra */
        j = k - 1;                                  /* reemplaza al peor */
    }
    while (j > 0 && mejorDist[j - 1] > d) {
        mejorDist[j] = mejorDist[j - 1];
        mejorIdx[j] = mejorIdx[j - 1];
        j--;
    }
    mejorDist[j] = d;
    mejorIdx[j] = id;
}

double knnTasaReconocimiento(const Dataset *train, const Dataset *test, int k) {
    if (!train || !test || train->nSamples <= 0 || test->nSamples <= 0)
        return 0.0;
    if (k < 1) k = 1;
    if (k > train->nSamples) k = train->nSamples;

    /* Memoria propia de cada llamada: cada hilo trabaja con sus arreglos */
    double *mejorDist = malloc((size_t)k * sizeof(double));
    int *mejorIdx = malloc((size_t)k * sizeof(int));
    if (!mejorDist || !mejorIdx) {
        free(mejorDist);
        free(mejorIdx);
        return 0.0;
    }

    int aciertos = 0;
    for (int t = 0; t < test->nSamples; t++) {
        const double *muestra = test->features[t];

        /* k vecinos mas cercanos */
        for (int i = 0; i < train->nSamples; i++) {
            double d = dist2(train->features[i], muestra, train->nFeatures);
            insertarVecino(mejorDist, mejorIdx, k, i < k ? i : k, d, i);
        }

        /* Votacion por mayoria (empate: la clase con menor id) */
        int votos[MAX_CLASES] = {0};
        for (int i = 0; i < k; i++)
            votos[train->labels[mejorIdx[i]]]++;

        int prediccion = 0;
        for (int c = 1; c < train->nClasses; c++)
            if (votos[c] > votos[prediccion]) prediccion = c;

        if (prediccion == test->labels[t]) aciertos++;
    }

    free(mejorDist);
    free(mejorIdx);
    return (double)aciertos / (double)test->nSamples;
}
