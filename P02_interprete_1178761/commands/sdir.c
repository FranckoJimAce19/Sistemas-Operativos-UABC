/**
 * @file  sdir.c
 * @brief Comando sdir — muestra el directorio de trabajo actual.
 *
 * Usa la funcion getcwd() de POSIX para obtener el directorio actual.
 */
#include <stdio.h>
#include <unistd.h>
#include "sdir.h"

/**
 * @brief Muestra el directorio de trabajo actual en la salida estandar.
 */
void sdir(void) {
    char cwd[512];

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        printf("sdir: error al obtener directorio\n");
    }
}
