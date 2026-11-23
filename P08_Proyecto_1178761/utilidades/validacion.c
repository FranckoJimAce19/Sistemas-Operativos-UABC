/**
 * @file  validacion.c
 * @brief Conversion estricta de argumentos a numero.
 */
#include <errno.h>
#include <stdlib.h>
#include "validacion.h"
#include "terminal.h"

int leerEntero(const char *comando, const char *nombre, const char *texto,
               int minimo, int maximo, int *valor) {
    if (texto == NULL || texto[0] == '\0') {
        mostrarError(comando, "falta el %s", nombre);
        return -1;
    }

    char *fin = NULL;
    errno = 0;
    long numero = strtol(texto, &fin, 10);

    if (fin == texto || *fin != '\0') {
        mostrarError(comando, "el %s '%s' no es un numero entero valido", nombre, texto);
        return -1;
    }
    if (errno == ERANGE || numero < minimo || numero > maximo) {
        mostrarError(comando, "el %s debe estar entre %d y %d (se recibio '%s')",
                     nombre, minimo, maximo, texto);
        return -1;
    }
    *valor = (int)numero;
    return 0;
}
