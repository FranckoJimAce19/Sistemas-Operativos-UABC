/**
 * @file  dataset.c
 * @brief Carga de los CSV del dataset — Practica 7 SO.
 */
#define _POSIX_C_SOURCE 200809L   /* strtok_r */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dataset.h"

#define LINEA_MAX 8192   /* longitud maxima de una fila del CSV */
#define COLS_MAX  1024   /* maximo de caracteristicas por fila */

/**
 * @brief Quita espacios y saltos de linea al inicio y al final, y comillas
 *        exteriores. Modifica la cadena y regresa el nuevo inicio.
 */
static char *limpiar(char *s) {
    while (*s && isspace((unsigned char)*s)) s++;
    size_t n = strlen(s);
    while (n > 0 && isspace((unsigned char)s[n - 1])) s[--n] = '\0';
    if (n >= 2 && (s[0] == '"' || s[0] == '\'') && s[n - 1] == s[0]) {
        s[n - 1] = '\0';
        s++;
    }
    return s;
}

/**
 * @brief Lee la siguiente linea no vacia de f (sin BOM ni espacios).
 * @return Puntero dentro de buf, o NULL al llegar al fin del archivo.
 */
static char *siguienteLinea(FILE *f, char *buf, int tam) {
    while (fgets(buf, tam, f)) {
        char *lin = buf;
        if ((unsigned char)lin[0] == 0xEF && (unsigned char)lin[1] == 0xBB &&
            (unsigned char)lin[2] == 0xBF)
            lin += 3;                                   /* BOM UTF-8 */
        lin = limpiar(lin);
        if (*lin != '\0') return lin;
    }
    return NULL;
}

/**
 * @brief Convierte una fila "v1,v2,..." a numeros.
 * @return Cantidad de valores leidos, o -1 si algun valor no es numerico.
 */
static int parsearFila(char *lin, double *vals) {
    int m = 0;
    char *save = NULL;
    for (char *tok = strtok_r(lin, ",", &save); tok; tok = strtok_r(NULL, ",", &save)) {
        char *t = limpiar(tok);
        char *fin = NULL;
        double v = strtod(t, &fin);
        if (fin == t || *fin != '\0') return -1;
        if (m == COLS_MAX) return -1;
        vals[m++] = v;
    }
    return m;
}

/**
 * @brief Carga un CSV de caracteristicas (X). Si la primera fila no es
 *        numerica se toma como encabezado y se salta.
 * @param huboEncabezado Se pone en 1 si se salto un encabezado.
 * @return 0 si ok, -1 en error.
 */
static int cargarX(const char *ruta, Dataset *d, int *huboEncabezado) {
    FILE *f = fopen(ruta, "r");
    if (!f) {
        printf("Error: no se pudo abrir '%s'.\n", ruta);
        return -1;
    }

    char linea[LINEA_MAX];
    double vals[COLS_MAX];
    double **filas = NULL;
    int n = 0, cap = 0, cols = -1, error = 0;
    char *lin;
    *huboEncabezado = 0;

    while (!error && (lin = siguienteLinea(f, linea, sizeof(linea)))) {
        int m = parsearFila(lin, vals);

        if (m < 0) {
            if (n == 0 && !*huboEncabezado) {          /* encabezado */
                *huboEncabezado = 1;
                continue;
            }
            printf("Error: valor no numerico en '%s' (fila %d).\n", ruta, n + 1);
            error = 1;
            break;
        }
        if (cols < 0) cols = m;
        if (m != cols) {
            printf("Error: la fila %d de '%s' tiene %d columnas, se esperaban %d.\n",
                   n + 1, ruta, m, cols);
            error = 1;
            break;
        }

        if (n == cap) {                                 /* crecer al doble */
            cap = cap ? cap * 2 : 1024;
            double **tmp = realloc(filas, (size_t)cap * sizeof(double *));
            if (!tmp) { error = 1; break; }
            filas = tmp;
        }
        filas[n] = malloc((size_t)cols * sizeof(double));
        if (!filas[n]) { error = 1; break; }
        memcpy(filas[n], vals, (size_t)cols * sizeof(double));
        n++;
    }
    fclose(f);

    if (!error && n == 0) {
        printf("Error: '%s' no contiene datos validos.\n", ruta);
        error = 1;
    }
    if (error) {
        for (int i = 0; i < n; i++) free(filas[i]);
        free(filas);
        return -1;
    }

    d->features = filas;
    d->nSamples = n;
    d->nFeatures = cols;
    return 0;
}

/**
 * @brief Busca una clase por nombre; si crear != 0 y no existe la agrega.
 * @return Id de la clase, o -1 si no existe y no se puede crear.
 */
static int claseId(Dataset *d, const char *nombre, int crear) {
    for (int i = 0; i < d->nClasses; i++)
        if (strcmp(d->clases[i], nombre) == 0) return i;
    if (!crear || d->nClasses >= MAX_CLASES) return -1;
    snprintf(d->clases[d->nClasses], MAX_NOMBRE_CLASE, "%s", nombre);
    return d->nClasses++;
}

/**
 * @brief Carga un CSV de etiquetas (y): se toma la primera columna de cada fila.
 *        Debe tener al menos d->nSamples filas; si tiene mas se recortan.
 * @param crearClases 1 para train (registra clases nuevas), 0 para test.
 * @param saltarEncabezado 1 si el X correspondiente tenia encabezado.
 * @return 0 si ok, -1 en error.
 */
static int cargarY(const char *ruta, Dataset *d, int crearClases, int saltarEncabezado) {
    FILE *f = fopen(ruta, "r");
    if (!f) {
        printf("Error: no se pudo abrir '%s'.\n", ruta);
        return -1;
    }

    int *labels = malloc((size_t)d->nSamples * sizeof(int));
    if (!labels) { fclose(f); return -1; }

    char linea[LINEA_MAX];
    char *lin;
    int n = 0, extra = 0, error = 0;

    while ((lin = siguienteLinea(f, linea, sizeof(linea)))) {
        if (saltarEncabezado) { saltarEncabezado = 0; continue; }

        char *coma = strchr(lin, ',');
        if (coma) *coma = '\0';
        lin = limpiar(lin);

        if (n == d->nSamples) { extra++; continue; }   /* filas sobrantes */

        int id = claseId(d, lin, crearClases);
        if (id < 0) {
            printf("Error: clase desconocida o demasiadas clases ('%s') en '%s'.\n", lin, ruta);
            error = 1;
            break;
        }
        labels[n++] = id;
    }
    fclose(f);

    if (!error && n < d->nSamples) {
        printf("Error: '%s' tiene %d etiquetas pero X tiene %d muestras.\n",
               ruta, n, d->nSamples);
        error = 1;
    }
    if (error) {
        free(labels);
        return -1;
    }
    if (extra > 0)
        printf("  Aviso: '%s' tiene %d etiquetas de mas; se ignoran las ultimas.\n",
               ruta, extra);

    d->labels = labels;
    return 0;
}

int datasetCargar(const char *dir, Dataset *train, Dataset *test) {
    memset(train, 0, sizeof(*train));
    memset(test, 0, sizeof(*test));

    char ruta[1024];
    int encabezado = 0;

    snprintf(ruta, sizeof(ruta), "%s/%s", dir, ARCHIVO_X_TRAIN);
    if (cargarX(ruta, train, &encabezado) != 0) goto error;
    snprintf(ruta, sizeof(ruta), "%s/%s", dir, ARCHIVO_Y_TRAIN);
    if (cargarY(ruta, train, 1, encabezado) != 0) goto error;

    snprintf(ruta, sizeof(ruta), "%s/%s", dir, ARCHIVO_X_TEST);
    if (cargarX(ruta, test, &encabezado) != 0) goto error;
    if (test->nFeatures != train->nFeatures) {
        printf("Error: test tiene %d caracteristicas y train %d.\n",
               test->nFeatures, train->nFeatures);
        goto error;
    }

    /* test usa la misma tabla de clases que train */
    memcpy(test->clases, train->clases, sizeof(train->clases));
    test->nClasses = train->nClasses;
    snprintf(ruta, sizeof(ruta), "%s/%s", dir, ARCHIVO_Y_TEST);
    if (cargarY(ruta, test, 0, encabezado) != 0) goto error;

    return 0;

error:
    datasetLiberar(train);
    datasetLiberar(test);
    return -1;
}

void datasetLiberar(Dataset *d) {
    if (!d) return;
    if (d->features) {
        for (int i = 0; i < d->nSamples; i++)
            free(d->features[i]);
        free(d->features);
    }
    free(d->labels);
    memset(d, 0, sizeof(*d));
}
