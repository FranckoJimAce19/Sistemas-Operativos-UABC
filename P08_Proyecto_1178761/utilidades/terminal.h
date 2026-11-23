/**
 * @file  terminal.h
 * @brief Salida en terminal: colores ANSI, mensajes estandarizados y
 *        elementos de formato (lineas separadoras y titulos).
 *
 * Los colores solo se emiten cuando la salida es una terminal real; si la
 * salida va a una tuberia o a un archivo (por ejemplo `lstprocess | grep`)
 * se imprimen cadenas vacias para no ensuciar el texto con codigos ANSI.
 */
#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdio.h>

/* Codigos de escape ANSI */
#define ANSI_RESET    "\033[0m"
#define ANSI_NEGRITA  "\033[1m"
#define ANSI_TENUE    "\033[2m"
#define ANSI_ROJO     "\033[1;31m"
#define ANSI_VERDE    "\033[1;32m"
#define ANSI_AMARILLO "\033[1;33m"
#define ANSI_AZUL     "\033[1;34m"
#define ANSI_MAGENTA  "\033[1;35m"
#define ANSI_CIAN     "\033[1;36m"
#define ANSI_BLANCO   "\033[1;37m"

/** Ancho estandar de los titulos y separadores del interprete. */
#define ANCHO_REPORTE 72

/**
 * @brief Devuelve el codigo ANSI si stdout es una terminal, o "" si no.
 * @param codigoAnsi Uno de los macros ANSI_*.
 */
const char *colorSalida(const char *codigoAnsi);

/**
 * @brief Igual que colorSalida pero evaluando stderr.
 */
const char *colorError(const char *codigoAnsi);

/**
 * @brief Imprime en stderr "comando: error: mensaje" (en rojo si aplica).
 * @param nombreComando Comando que reporta el error (puede ser NULL).
 * @param formato       Formato estilo printf.
 */
void mostrarError(const char *nombreComando, const char *formato, ...)
    __attribute__((format(printf, 2, 3)));

/**
 * @brief 1 si se reporto algun error desde la ultima llamada a
 *        reiniciarErrorComando() (el prompt marca ">>>" en rojo).
 */
int huboErrorComando(void);

/** @brief Olvida los errores reportados (se llama antes de cada comando). */
void reiniciarErrorComando(void);

/** @brief Marca el comando actual como fallido sin imprimir nada. */
void marcarErrorComando(void);

/**
 * @brief Imprime en stdout "aviso: mensaje" en amarillo.
 */
void mostrarAviso(const char *formato, ...) __attribute__((format(printf, 1, 2)));

/**
 * @brief Imprime en stdout un mensaje de exito precedido de una marca verde.
 */
void mostrarExito(const char *formato, ...) __attribute__((format(printf, 1, 2)));

/**
 * @brief Imprime la forma de uso correcta de un comando en stderr.
 * @param sintaxis Texto de sintaxis, por ejemplo "copy <origen> <destino>".
 */
void mostrarUso(const char *sintaxis);

/**
 * @brief Imprime una linea horizontal repitiendo un caracter (UTF-8).
 * @param caracter Caracter a repetir (por ejemplo "─" o "=").
 * @param ancho    Cantidad de repeticiones.
 */
void imprimirLineaSeparadora(const char *caracter, int ancho);

/**
 * @brief Imprime un titulo de seccion entre dos lineas dobles.
 */
void imprimirTituloPrincipal(const char *titulo);

/**
 * @brief Imprime un subtitulo con una linea simple debajo.
 */
void imprimirSubtitulo(const char *subtitulo);

/**
 * @brief Descripcion en espanol de un codigo errno (para los errores mas
 *        comunes; el resto se describe con strerror).
 */
const char *describirErrorSistema(int numeroError);

/**
 * @brief Columnas de la terminal (80 si no se puede determinar).
 */
int obtenerAnchoTerminal(void);

#endif /* TERMINAL_H */
