/**
 * @file  del.c
 * @brief Comando del — elimina un archivo via syscall unlink() de POSIX.
 *
 *  Sintaxis: del [archivo]
 *  Ejemplo:  del archivo.txt
 */
#define _DEFAULT_SOURCE   /* lstat */
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include "../utilidades/terminal.h"
#include <sys/stat.h>
#include "del.h"

/**
 * @brief Elimina el archivo usando unlink().
 */
void del(const char *filePath) {
    if (filePath == NULL || filePath[0] == '\0') {
        mostrarError("del", "faltan argumentos");
        mostrarUso("del <archivo> [archivo ...]");
        return;
    }

    struct stat info;
    if (lstat(filePath, &info) != 0) {
        mostrarError("del", "'%s': %s", filePath, describirErrorSistema(errno));
        return;
    }
    if (S_ISDIR(info.st_mode)) {
        mostrarError("del", "'%s' es un directorio (del solo elimina archivos)", filePath);
        return;
    }

    if (unlink(filePath) == 0)
        mostrarExito("'%s' eliminado", filePath);
    else
        mostrarError("del", "no se pudo eliminar '%s': %s", filePath, describirErrorSistema(errno));
}
