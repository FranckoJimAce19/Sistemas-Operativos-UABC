/**
 * @file  del.c
 * @brief Comando del — elimina un archivo via syscall unlink() de POSIX.
 *
 *  Sintaxis: del [archivo]
 *  Ejemplo:  del archivo.txt
 */
#include <unistd.h>
#include <stdio.h>
#include "del.h"

/**
 * @brief Elimina el archivo usando unlink().
 */
void del(const char *filePath) {
    if (filePath == NULL || filePath[0] == '\0') {
        printf("Error: del requiere un archivo.\n");
        printf("Uso: del [archivo]\n");
        return;
    }

    if (unlink(filePath) == 0) {
        printf("Archivo '%s' eliminado correctamente.\n", filePath);
    } else {
        printf("Error: no se pudo eliminar '%s'. Verifique que exista.\n", filePath);
    }
}
