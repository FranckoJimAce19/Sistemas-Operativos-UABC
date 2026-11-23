/**
 * @file  clear.c
 * @brief Comando clear — limpia la pantalla de la terminal.
 *
 * Usa secuencias de escape ANSI:
 *   \033[2J — limpia toda la pantalla.
 *   \033[H  — mueve el cursor a la esquina superior izquierda.
 *   \033[3J — limpia el scrollback buffer (historial de terminal).
 */
#include <stdio.h>
#include "clear.h"

/**
 * @brief Limpia la pantalla de la terminal.
 */
void clearScreen(void) {
    printf("\033[2J\033[H\033[3J");
    fflush(stdout);
}
