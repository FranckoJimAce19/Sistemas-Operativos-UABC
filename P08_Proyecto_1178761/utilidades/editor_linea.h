/**
 * @file  editor_linea.h
 * @brief Lectura de lineas con edicion, historial y autocompletado.
 *
 * Cuando la entrada y la salida son una terminal se usa un editor propio
 * en modo raw (termios), sin bibliotecas externas:
 *
 *   Flechas izquierda/derecha, Inicio/Fin, Ctrl+A/Ctrl+E   mover el cursor
 *   Flechas arriba/abajo, Ctrl+P/Ctrl+N                     recorrer el historial
 *   Retroceso, Supr                                         borrar caracteres
 *   Ctrl+U / Ctrl+K                                         borrar hasta el inicio / el final
 *   Ctrl+W                                                  borrar la palabra anterior
 *   TAB                                                     autocompletar (doble TAB lista opciones)
 *   Ctrl+L                                                  limpiar la pantalla
 *   Ctrl+C                                                  descartar la linea
 *   Ctrl+D                                                  fin de entrada (con la linea vacia)
 *
 * Si la entrada no es una terminal (por ejemplo un script redirigido) se
 * lee linea por linea con fgets y no se muestra el prompt.
 */
#ifndef EDITOR_LINEA_H
#define EDITOR_LINEA_H

#include <stddef.h>

/** Resultado de leer una linea. */
typedef enum {
    LECTURA_EXITOSA,      /**< se leyo una linea (puede estar vacia) */
    LECTURA_CANCELADA,    /**< el usuario presiono Ctrl+C */
    LECTURA_FIN_ENTRADA   /**< Ctrl+D con la linea vacia, o fin del archivo */
} ResultadoLectura;

/**
 * @brief Inicializa el editor (guarda la configuracion original de la
 *        terminal para restaurarla siempre al salir).
 */
void editorInicializar(void);

/**
 * @brief 1 si se usa el editor interactivo (entrada y salida son terminal).
 */
int editorInteractivo(void);

/**
 * @brief Lee una linea mostrando el prompt.
 * @param prompt               Texto del prompt (puede tener codigos ANSI).
 * @param destino              Buffer de salida (sin salto de linea final).
 * @param tamanoDestino        Capacidad del buffer.
 * @param usarHistorial        1 para habilitar flechas de historial y TAB.
 */
ResultadoLectura editorLeerLinea(const char *prompt, char *destino, size_t tamanoDestino,
                           int usarHistorial);

/**
 * @brief Restaura la configuracion original de la terminal.
 */
void editorRestaurar(void);

#endif /* EDITOR_LINEA_H */
