/**
 * @file  cmd_knn.h
 * @brief Comando 'knn' del interprete: busqueda de hiper-parametros K-NN.
 */
#ifndef CMD_KNN_H
#define CMD_KNN_H

/**
 * @brief Ejecuta la busqueda K-NN con hilos.
 *        Uso: knn [directorio] [hilos]
 *        - directorio: donde estan los 4 CSV (por defecto "data")
 *        - hilos: 1, 2 o 3 (si se omite, se reportan las tres corridas)
 */
void cmdKnn(char **args);

#endif /* CMD_KNN_H */
