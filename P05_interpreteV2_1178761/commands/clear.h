/**
 * @file  clear.h
 * @brief Comando clear — limpia la pantalla de la terminal.
 */
#ifndef CLEAR_H
#define CLEAR_H

/**
 * @brief Limpia la pantalla usando secuencias de escape ANSI.
 *        Limpia tanto la pantalla visible como el scrollback buffer.
 */
void clearScreen(void);

#endif /* CLEAR_H */
