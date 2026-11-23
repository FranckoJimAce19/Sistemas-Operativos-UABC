/**
 * @file  autocompletado.h
 * @brief Candidatos de autocompletado para la tecla TAB.
 *
 * - En la posicion de comando (inicio de linea o despues de '|') se
 *   sugieren comandos internos y programas del PATH.
 * - En cualquier otra posicion se sugieren archivos y directorios
 *   (los directorios terminan en '/').
 */
#ifndef AUTOCOMPLETADO_H
#define AUTOCOMPLETADO_H

#include <stddef.h>

/** Lista dinamica de candidatos (ordenada y sin repetidos). */
typedef struct {
    char **candidatos;
    size_t cantidad;
    size_t capacidad;
} ListaCandidatos;

/**
 * @brief Calcula los candidatos para la palabra que termina en el cursor.
 * @param linea          Texto completo de la linea.
 * @param posicionCursor Posicion del cursor (bytes).
 * @param lista          Destino (debe liberarse con liberarCandidatos).
 * @return Posicion donde inicia la palabra a reemplazar.
 */
size_t buscarCandidatosCompletado(const char *linea, size_t posicionCursor,
                                  ListaCandidatos *lista);

/**
 * @brief Longitud del prefijo comun a todos los candidatos.
 */
size_t calcularPrefijoComun(const ListaCandidatos *lista);

/**
 * @brief Libera los candidatos y deja la lista vacia.
 */
void liberarCandidatos(ListaCandidatos *lista);

#endif /* AUTOCOMPLETADO_H */
