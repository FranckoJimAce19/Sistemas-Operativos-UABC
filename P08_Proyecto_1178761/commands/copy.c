/**
 * @file  copy.c
 * @brief Comando copy — copia el contenido de un archivo a otro via syscalls POSIX.
 *
 *  Sintaxis: copy [archivo_fuente] [archivo_destino]
 *  Ejemplo:  copy archivo1.txt archivo2.txt
 */
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "../utilidades/terminal.h"
#include <sys/stat.h>
#include "copy.h"

#define BUFFER_SIZE 1024

/**
 * @brief Copia el contenido del archivo origen al destino.
 *        Usa open/read/write/close para manejo POSIX de archivos.
 */
void copy(const char *source, const char *destination) {
    if (source == NULL || source[0] == '\0' || destination == NULL || destination[0] == '\0') {
        mostrarError("copy", "faltan argumentos");
        mostrarUso("copy <origen> <destino>");
        return;
    }

    struct stat infoOrigen, infoDestino;
    char rutaDestino[4096];
    if (stat(source, &infoOrigen) != 0) {
        mostrarError("copy", "'%s': %s", source, describirErrorSistema(errno));
        return;
    }
    if (S_ISDIR(infoOrigen.st_mode)) {
        mostrarError("copy", "'%s' es un directorio", source);
        return;
    }
    /* Si el destino es un directorio se copia dentro con el mismo nombre */
    if (stat(destination, &infoDestino) == 0 && S_ISDIR(infoDestino.st_mode)) {
        const char *nombre = strrchr(source, '/');
        size_t largo = strlen(destination);
        int terminaEnDiagonal = largo > 0 && destination[largo - 1] == '/';
        snprintf(rutaDestino, sizeof(rutaDestino), "%s%s%s", destination,
                 terminaEnDiagonal ? "" : "/", nombre ? nombre + 1 : source);
        destination = rutaDestino;
    }
    if (stat(destination, &infoDestino) == 0) {
        if (S_ISDIR(infoDestino.st_mode)) {
            mostrarError("copy", "'%s' es un directorio", destination);
            return;
        }
        /* copiar un archivo sobre si mismo lo dejaria vacio */
        if (infoDestino.st_dev == infoOrigen.st_dev && infoDestino.st_ino == infoOrigen.st_ino) {
            mostrarError("copy", "'%s' y '%s' son el mismo archivo", source, destination);
            return;
        }
    }

    int fdInput = open(source, O_RDONLY);
    if (fdInput == -1) {
        mostrarError("copy", "no se pudo abrir '%s': %s", source, describirErrorSistema(errno));
        return;
    }

    int fdOutput = open(destination, O_WRONLY | O_CREAT | O_TRUNC, infoOrigen.st_mode & 0777);
    if (fdOutput == -1) {
        mostrarError("copy", "no se pudo crear '%s': %s", destination, describirErrorSistema(errno));
        close(fdInput);
        return;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
    long totalBytes = 0;
    int huboError = 0;

    while ((bytesRead = read(fdInput, buffer, sizeof(buffer))) > 0) {
        if (write(fdOutput, buffer, bytesRead) != bytesRead) {
            mostrarError("copy", "fallo al escribir en '%s'", destination);
            huboError = 1;
            break;
        }
        totalBytes += bytesRead;
    }
    if (bytesRead < 0) {
        mostrarError("copy", "fallo al leer '%s'", source);
        huboError = 1;
    }

    close(fdInput);
    close(fdOutput);

    if (!huboError)
        mostrarExito("'%s' -> '%s' (%ld bytes)", source, destination, totalBytes);
}
