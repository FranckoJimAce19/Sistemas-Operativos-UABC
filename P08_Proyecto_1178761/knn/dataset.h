/**
 * @file  dataset.h
 * @brief Carga del dataset K-NN (4 archivos CSV).
 *        Formato: valores separados por coma, una muestra por fila.
 *        Las etiquetas (y) pueden ser numeros o texto; se mapean a ids 0..n-1.
 */
#ifndef DATASET_H
#define DATASET_H

/* Nombres de los 4 archivos CSV del dataset (carpeta data/) */
#define ARCHIVO_X_TRAIN "x_trn_features_var_selected.csv"
#define ARCHIVO_Y_TRAIN "y_trn.csv"
#define ARCHIVO_X_TEST  "x_tst_features_var_selected.csv"
#define ARCHIVO_Y_TEST  "y_tst.csv"

#define MAX_CLASES       64   /* limite de clases distintas */
#define MAX_NOMBRE_CLASE 32   /* longitud maxima del nombre de una clase */

/**
 * @brief Conjunto de datos en memoria.
 */
typedef struct {
    double **features;   /**< matriz nSamples x nFeatures */
    int *labels;         /**< id de clase de cada muestra (indice en clases[]) */
    int nSamples;        /**< cantidad de muestras */
    int nFeatures;       /**< cantidad de caracteristicas */
    int nClasses;        /**< cantidad de clases distintas */
    char clases[MAX_CLASES][MAX_NOMBRE_CLASE]; /**< etiqueta original por id */
} Dataset;

/**
 * @brief Carga los 4 CSV de un directorio (entrenamiento y prueba).
 *        Si un archivo y tiene mas filas que su X se recorta y se avisa.
 * @return 0 si todo cargo correctamente, -1 en caso de error.
 */
int datasetCargar(const char *dir, Dataset *train, Dataset *test);

/**
 * @brief Libera toda la memoria asociada al dataset y lo deja en cero.
 */
void datasetLiberar(Dataset *d);

#endif /* DATASET_H */
