/**
 * @file  memoria.h
 * @brief Memoria principal simulada con asignacion contigua
 *        (first fit, best fit y worst fit) y compactacion.
 *
 * Cada bloque guarda el pid del proceso que lo ocupa (0 = libre).
 */
#ifndef MEMORIA_H
#define MEMORIA_H

#define TOTAL_BLOQUES   100   /* bloques de la memoria principal */
#define BLOQUES_FILA    10    /* bloques por renglon en mstatus */
#define BLOQUE_LIBRE    0

typedef enum {
    FIRST_FIT,   /* primer hueco suficiente */
    BEST_FIT,    /* hueco suficiente mas pequeno */
    WORST_FIT    /* hueco mas grande */
} Estrategia;

typedef struct {
    int bloques[TOTAL_BLOQUES];   /* pid dueno de cada bloque */
} Memoria;

/** Memoria global del interprete. */
extern Memoria memoriaGlobal;

void memoriaInicializar(Memoria *mem);

/**
 * @brief Convierte "first", "best", "worst" (o ff, bf, wf) a Estrategia.
 * @return 0 si ok, -1 si no es una estrategia valida.
 */
int memoriaEstrategia(const char *texto, Estrategia *estrategia);

/** @brief Nombre de la estrategia ("first fit", ...). */
const char *estrategiaTexto(Estrategia estrategia);

/**
 * @brief Carga un proceso en un hueco contiguo segun la estrategia.
 * @param inicio Primer bloque asignado (si hubo exito).
 * @return 0 si se asigno, -1 si no hay memoria suficiente,
 *         -2 si hay bloques libres pero ningun hueco contiguo basta.
 */
int memoriaAsignar(Memoria *mem, int pid, int tamano, Estrategia estrategia, int *inicio);

/** @brief Libera los bloques del proceso. @return bloques liberados. */
int memoriaLiberar(Memoria *mem, int pid);

/** @brief 1 si el proceso esta en memoria (y guarda su primer bloque). */
int memoriaBuscar(const Memoria *mem, int pid, int *inicio);

/** @brief Cantidad de bloques libres. */
int memoriaLibres(const Memoria *mem);

/** @brief 1 si hay huecos entre procesos (compactar serviria). */
int memoriaFragmentada(const Memoria *mem);

/** @brief Junta los procesos al inicio. @return procesos que se movieron. */
int memoriaCompactar(Memoria *mem);

/** @brief Color del proceso en el mapa (vacio si no es terminal). */
const char *memoriaColorProceso(int pid);

/** @brief Imprime el mapa de bloques, los segmentos y el resumen (mstatus). */
void memoriaMostrar(const Memoria *mem);

#endif /* MEMORIA_H */
