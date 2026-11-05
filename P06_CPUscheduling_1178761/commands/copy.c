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
#include <sys/stat.h>
#include "copy.h"

#define BUFFER_SIZE 1024

/**
 * @brief Copia el contenido del archivo origen al destino.
 *        Usa open/read/write/close para manejo POSIX de archivos.
 */
void copy(const char *source, const char *destination) {
    if (source == NULL || source[0] == '\0' || destination == NULL || destination[0] == '\0') {
        printf("Error: copy requiere dos argumentos.\n");
        printf("Uso: copy [origen] [destino]\n");
        return;
    }

    int fdInput = open(source, O_RDONLY);
    if (fdInput == -1) {
        printf("Error: no se pudo abrir '%s'.\n", source);
        return;
    }

    int fdOutput = open(destination, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fdOutput == -1) {
        printf("Error: no se pudo crear '%s'.\n", destination);
        close(fdInput);
        return;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
    long totalBytes = 0;

    while ((bytesRead = read(fdInput, buffer, sizeof(buffer))) > 0) {
        write(fdOutput, buffer, bytesRead);
        totalBytes += bytesRead;
    }

    close(fdInput);
    close(fdOutput);

    printf("Archivo copiado: '%s' -> '%s' (%ld bytes).\n", source, destination, totalBytes);
}
