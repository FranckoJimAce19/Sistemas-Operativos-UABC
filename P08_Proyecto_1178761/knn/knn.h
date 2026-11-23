/**
 * @file  knn.h
 * @brief Clasificador K-NN (k-vecinos mas cercanos).
 */
#ifndef KNN_H
#define KNN_H

#include "dataset.h"

/**
 * @brief Tasa de reconocimiento (precision) del clasificador sobre el test.
 * @return Valor en [0.0, 1.0]; 0.0 si no hay muestras de prueba.
 */
double knnTasaReconocimiento(const Dataset *train, const Dataset *test, int k);

#endif /* KNN_H */
