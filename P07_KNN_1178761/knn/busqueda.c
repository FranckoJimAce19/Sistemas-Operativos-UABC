/**
 * @file  busqueda.c
 * @brief Busqueda de hiper-parametros K-NN con pthreads — Practica 7 SO.
 *        Sincronizacion:
 *          - barrera: todos los hilos arrancan al mismo tiempo.
 *          - mutex:   reparte los valores de k (cada k se evalua una sola vez).
 */
#define _POSIX_C_SOURCE 200809L   /* clock_gettime, pthread_barrier_t */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "busqueda.h"
#include "knn.h"

const int K_VALORES[NUM_K] = {1, 3, 5, 7, 9, 11, 13, 15, 17};
int g_numHilos = MAX_HILOS;

/** Datos compartidos por todos los hilos de una corrida. */
typedef struct {
    const Dataset *train;
    const Dataset *test;
    int siguienteK;             /* indice del proximo k por evaluar */
    pthread_mutex_t mutex;      /* protege siguienteK y rep->completados */
    pthread_barrier_t barrera;  /* sincroniza el inicio de todos los hilos */
    ReporteBusqueda *rep;
} Compartido;

/**
 * @brief Hilo trabajador: espera en la barrera y luego toma k's libres
 *        bajo el mutex hasta agotar el espacio de busqueda.
 */
static void *hiloBuscar(void *arg) {
    Compartido *c = (Compartido *)arg;

    pthread_barrier_wait(&c->barrera);

    for (;;) {
        /* Seccion critica: reservar el siguiente k */
        pthread_mutex_lock(&c->mutex);
        int i = c->siguienteK;
        if (i < NUM_K) c->siguienteK++;
        pthread_mutex_unlock(&c->mutex);

        if (i >= NUM_K) break;

        /* Cada hilo escribe solo en resultados[i], que ningun otro hilo toca. */
        c->rep->resultados[i].k = K_VALORES[i];
        c->rep->resultados[i].tasa = knnTasaReconocimiento(c->train, c->test, K_VALORES[i]);

        pthread_mutex_lock(&c->mutex);
        c->rep->completados++;
        pthread_mutex_unlock(&c->mutex);
    }
    return NULL;
}

/** @brief Tiempo monotono actual en segundos. */
static double ahoraSeg(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

int busquedaEjecutar(const Dataset *train, const Dataset *test, ReporteBusqueda *rep) {
    if (!train || !test || !rep) return -1;

    int numHilos = g_numHilos;
    if (numHilos < 1) numHilos = 1;
    if (numHilos > NUM_K) numHilos = NUM_K;   /* no mas hilos que valores de k */

    memset(rep, 0, sizeof(*rep));

    Compartido c = { .train = train, .test = test, .siguienteK = 0, .rep = rep };
    pthread_t hilos[NUM_K];

    pthread_mutex_init(&c.mutex, NULL);
    pthread_barrier_init(&c.barrera, NULL, (unsigned)numHilos);

    double t0 = ahoraSeg();
    for (int h = 0; h < numHilos; h++) {
        if (pthread_create(&hilos[h], NULL, hiloBuscar, &c) != 0) {
            /* Los hilos ya creados esperan en la barrera a uno que nunca llegara, asi que no hay forma limpia de continuar. */
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }
    for (int h = 0; h < numHilos; h++)
        pthread_join(hilos[h], NULL);
    rep->tiempoSeg = ahoraSeg() - t0;

    pthread_barrier_destroy(&c.barrera);
    pthread_mutex_destroy(&c.mutex);

    /* k optimo (empate: el menor k) */
    rep->mejorK = rep->resultados[0].k;
    rep->mejorTasa = rep->resultados[0].tasa;
    for (int i = 1; i < NUM_K; i++) {
        if (rep->resultados[i].tasa > rep->mejorTasa) {
            rep->mejorTasa = rep->resultados[i].tasa;
            rep->mejorK = rep->resultados[i].k;
        }
    }
    return 0;
}

/* ------------------------------ Salida ------------------------------ */

#define ANCHO 56   /* ancho de las lineas separadoras */

/** @brief Imprime una linea de 'n' veces el caracter (UTF-8) c. */
static void linea(const char *c, int n) {
    printf("  ");
    for (int i = 0; i < n; i++) fputs(c, stdout);
    printf("\n");
}

/** @brief Titulo de seccion con su linea debajo. */
static void titulo(const char *texto) {
    printf("\n  %s\n", texto);
    linea("─", ANCHO);
}

/** @brief Resultado de una corrida: tabla de tasa por k (marca el k
 *         optimo), tiempo y k optimo de esa corrida. */
static void reportarCorrida(const ReporteBusqueda *rep, int hilos) {
    printf("  %5s   %8s\n", "k", "Tasa");
    for (int i = 0; i < NUM_K; i++) {
        const ResultadoK *r = &rep->resultados[i];
        printf("  %5d   %7.2f %%%s\n", r->k, r->tasa * 100.0,
               r->k == rep->mejorK ? "   <- mejor" : "");
    }
    linea("·", ANCHO);
    printf("  Tiempo: %.2f s  |  %d hilo%s  |  %d/%d k evaluados\n",
           rep->tiempoSeg, hilos, hilos > 1 ? "s" : "", rep->completados, NUM_K);
    printf("  k optimo: %d  (%.2f %%)\n", rep->mejorK, rep->mejorTasa * 100.0);
}

/** @brief Tabla de tiempos con speedup y eficiencia respecto a 1 hilo. */
static void reportarTiempos(const double *tiempos, int desde, int hasta) {
    titulo("Tiempo de la busqueda");
    printf("  %5s   %10s   %8s   %10s\n", "Hilos", "Tiempo", "Speedup", "Eficiencia");
    for (int h = desde; h <= hasta; h++) {
        printf("  %5d   %8.2f s", h, tiempos[h]);
        if (tiempos[1] > 0.0 && tiempos[h] > 0.0) {
            double speedup = tiempos[1] / tiempos[h];
            printf("   %7.2fx   %8.1f %%", speedup, speedup / h * 100.0);
        }
        printf("\n");
    }
}

/** @brief 1 si todas las corridas dieron exactamente las mismas tasas. */
static int tasasIguales(const ReporteBusqueda *reps, int desde, int hasta) {
    for (int h = desde + 1; h <= hasta; h++)
        for (int i = 0; i < NUM_K; i++)
            if (reps[h].resultados[i].tasa != reps[desde].resultados[i].tasa)
                return 0;
    return 1;
}

int busquedaPractica(const char *dir, int hilos) {
    printf("\n");
    linea("═", ANCHO);
    printf("  Practica 7  |  Busqueda de hiper-parametros K-NN\n");
    linea("═", ANCHO);

    printf("  Cargando dataset de '%s' ...\n", dir);
    fflush(stdout);
    Dataset train, test;
    if (datasetCargar(dir, &train, &test) != 0)
        return -1;

    /* Sin normalizar: las caracteristicas ya vienen preprocesadas. Con
       min-max la tasa bajaba de ~88% a ~77%. */
    titulo("Dataset");
    printf("  %-15s %d muestras x %d caracteristicas\n",
           "Entrenamiento", train.nSamples, train.nFeatures);
    printf("  %-15s %d muestras\n", "Prueba", test.nSamples);
    printf("  %-15s %d\n", "Clases", train.nClasses);
    printf("  %-15s {", "Valores de k");
    for (int i = 0; i < NUM_K; i++)
        printf("%d%s", K_VALORES[i], i < NUM_K - 1 ? ", " : "}\n");

    int desde = hilos > 0 ? hilos : 1;
    int hasta = hilos > 0 ? hilos : MAX_HILOS;
    double tiempos[MAX_HILOS + 1] = {0.0};
    ReporteBusqueda reps[MAX_HILOS + 1];
    int resultado = 0;

    for (int h = desde; h <= hasta; h++) {
        char nombre[32];
        snprintf(nombre, sizeof(nombre), "Corrida con %d hilo%s", h, h > 1 ? "s" : "");
        titulo(nombre);
        if (isatty(STDOUT_FILENO)) {        /* solo en terminal, no en archivo */
            printf("  Calculando ...\r");   /* la tabla lo sobrescribe */
            fflush(stdout);
        }

        g_numHilos = h;
        if (busquedaEjecutar(&train, &test, &reps[h]) != 0) {
            printf("  Error en la corrida.\n");
            resultado = -1;
            break;
        }
        tiempos[h] = reps[h].tiempoSeg;
        reportarCorrida(&reps[h], h);
    }

    if (resultado == 0) {
        const ReporteBusqueda *rep = &reps[hasta];
        reportarTiempos(tiempos, desde, hasta);

        printf("\n");
        linea("═", ANCHO);
        printf("  k optimo: %d   |   tasa de reconocimiento: %.2f %%\n",
               rep->mejorK, rep->mejorTasa * 100.0);
        if (hasta > desde)
            printf("  Tasas identicas en las %d corridas: %s\n", hasta - desde + 1,
                   tasasIguales(reps, desde, hasta) ? "si" : "NO (revisar sincronizacion)");
        linea("═", ANCHO);
        printf("\n");
    }

    datasetLiberar(&train);
    datasetLiberar(&test);
    return resultado;
}
