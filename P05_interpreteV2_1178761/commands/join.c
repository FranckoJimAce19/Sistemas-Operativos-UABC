/**
 * @file  join.c
 * @brief Comando join — muestra o crea archivos de texto via syscalls POSIX.
 *
 *  join [archivo]   → Lee y muestra el contenido del archivo.
 *  join> [archivo]  → Entra en modo texto; guarda lo escrito en el archivo
 *                     hasta que el usuario escriba '$' y presione Enter.
 */
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "join.h"

#define BUFFER_SIZE 1024

/**
 * @brief Muestra el contenido de un archivo usando open/read/write/close.
 */
void joinRead(const char *filePath) {
    if (filePath == NULL || filePath[0] == '\0') {
        printf("Error: join requiere un archivo.\n");
        printf("Uso: join [archivo]\n");
        return;
    }

    int fd = open(filePath, O_RDONLY);
    if (fd == -1) {
        printf("Error: no se pudo abrir '%s'.\n", filePath);
        return;
    }

    char buf[BUFFER_SIZE];
    ssize_t n;

    printf("\n--- Contenido de '%s' ---\n", filePath);
    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        write(STDOUT_FILENO, buf, n);
    }
    printf("--- Fin de '%s' ---\n\n", filePath);

    close(fd);
}

/**
 * @brief Crea un archivo en modo texto. El usuario escribe lineas hasta '$'.
 *        Usa open/write/close para manejo POSIX de archivos.
 */
void joinWrite(const char *filePath) {
    if (filePath == NULL || filePath[0] == '\0') {
        printf("Error: join> requiere un archivo.\n");
        printf("Uso: join> [archivo]\n");
        return;
    }

    int fd = open(filePath, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        printf("Error: no se pudo crear '%s'.\n", filePath);
        return;
    }

    printf("Escribe el contenido. Para guardar escribe '$' y presiona Enter.\n");

    char line[BUFFER_SIZE];
    while (1) {
        printf("> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }

        // Eliminar salto de linea 
        line[strcspn(line, "\n")] = '\0';

        // Buscar '$' en cualquier posicion de la linea 
        char *dollarSign = strchr(line, '$');
        if (dollarSign != NULL) {
            // Si hay contenido antes de '$', guardarlo 
            if (dollarSign > line) {
                *dollarSign = '\0';
                write(fd, line, strlen(line));
                write(fd, "\n", 1);
            }
            break;
        }

        // Escribir linea al archivo 
        write(fd, line, strlen(line));
        write(fd, "\n", 1);
    }

    close(fd);
    printf("Archivo '%s' guardado correctamente.\n", filePath);
}
