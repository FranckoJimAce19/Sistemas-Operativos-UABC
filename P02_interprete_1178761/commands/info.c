/**
 * @file  info.c
 * @brief Comando info — muestra todos los comandos disponibles del interprete.
 *        Lee la ayuda desde commands/info.txt para facilitar actualizaciones.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <libgen.h>
#include <string.h>
#include "info.h"

/**
 * @brief Lee y muestra el archivo de ayuda usando /proc/self/exe
 *        para encontrar la ruta relativa al ejecutable.
 */
void info(void) {
    char exe[512];
    ssize_t len = readlink("/proc/self/exe", exe, sizeof(exe) - 1);
    if (len == -1) {
        printf("Error: no se pudo obtener ruta del ejecutable.\n");
        return;
    }
    exe[len] = '\0';

    /* Construir ruta a info.txt relativa al ejecutable */
    char path[512];
    snprintf(path, sizeof(path), "%s/commands/info.txt", dirname(exe));

    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        printf("Error: no se pudo abrir '%s'.\n", path);
        return;
    }

    char buf[1024];
    ssize_t n;
    printf("\n");
    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        write(STDOUT_FILENO, buf, n);
    }
    printf("\n");
    close(fd);
}
