/**
 * @file  validacion.h
 * @brief Conversion estricta de argumentos a numero (a diferencia de atoi,
 *        rechaza textos como "12abc" o "" y valores fuera de rango).
 */
#ifndef VALIDACION_H
#define VALIDACION_H

/**
 * @brief Convierte texto a entero dentro de [minimo, maximo]. Si no es
 *        valido imprime "comando: error: el <nombre> ..." y regresa -1.
 * @param comando Comando que valida (para el mensaje).
 * @param nombre  Nombre del argumento (ej. "burst time").
 * @return 0 si el valor es valido, -1 si no.
 */
int leerEntero(const char *comando, const char *nombre, const char *texto,
               int minimo, int maximo, int *valor);

#endif /* VALIDACION_H */
