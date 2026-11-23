/**
 * @file  sdir.c
 * @brief Comando sdir — muestra el directorio de trabajo actual.
 *
 * Usa la funcion getcwd() de POSIX para obtener el directorio actual.
 */
#include <stdio.h>
#include <errno.h>
#include "../utilidades/terminal.h"
#include <unistd.h>
#include "sdir.h"

/**
 * @brief Muestra el directorio de trabajo actual en la salida estandar.
 */
void sdir(void) {
    char cwd[4096];

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        mostrarError("sdir", "no se pudo obtener el directorio actual: %s",
                     describirErrorSistema(errno));
    }
}
