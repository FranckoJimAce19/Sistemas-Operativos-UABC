/**
 * @file  join.c
 * @brief Comando join — muestra o crea archivos de texto via syscalls POSIX.
 *
 *  join [archivo]   → Lee y muestra el contenido del archivo.
 *  join> [archivo]  → Entra en modo texto; guarda lo escrito en el archivo
 *                     hasta que el usuario escriba '$' y presione Enter.
 */
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "join.h"
#include "../utilidades/editor_linea.h"
#include "../utilidades/terminal.h"

#define BUFFER_SIZE 1024

/**
 * @brief Muestra el contenido de un archivo usando open/read/write/close.
 */
void joinRead(const char *filePath) {
    if (filePath == NULL || filePath[0] == '\0') {
        mostrarError("join", "faltan argumentos");
        mostrarUso("join <archivo>");
        return;
    }

    struct stat info;
    if (stat(filePath, &info) != 0) {
        mostrarError("join", "'%s': %s", filePath, describirErrorSistema(errno));
        return;
    }
    if (S_ISDIR(info.st_mode)) {
        mostrarError("join", "'%s' es un directorio", filePath);
        return;
    }

    int fd = open(filePath, O_RDONLY);
    if (fd == -1) {
        mostrarError("join", "no se pudo abrir '%s': %s", filePath, describirErrorSistema(errno));
        return;
    }

    /* Los encabezados solo se muestran en terminal, para que no se mezclen
       con el contenido en una tuberia (join f | grep x) */
    int enTerminal = isatty(STDOUT_FILENO);
    if (enTerminal)
        printf("%s─── %s ───%s\n", colorSalida(ANSI_CIAN), filePath, colorSalida(ANSI_RESET));
    fflush(stdout);

    char buf[BUFFER_SIZE];
    ssize_t n;
    char ultimo = '\n';
    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        if (write(STDOUT_FILENO, buf, n) < 0) break;
        ultimo = buf[n - 1];
    }
    close(fd);

    if (enTerminal) {
        if (ultimo != '\n') printf("\n");   /* que el pie no quede pegado al texto */
        printf("%s─── fin de %s (%lld bytes) ───%s\n", colorSalida(ANSI_CIAN), filePath,
               (long long)info.st_size, colorSalida(ANSI_RESET));
    }
}

/**
 * @brief Agrega una linea mas su salto al texto acumulado.
 * @return 0 si ok, -1 si no hay memoria.
 */
static int agregarLinea(char **texto, size_t *largo, size_t *capacidad, const char *linea,
                        size_t n) {
    if (*largo + n + 1 > *capacidad) {
        size_t nueva = *capacidad ? *capacidad * 2 : 1024;
        while (nueva < *largo + n + 1) nueva *= 2;
        char *ampliado = realloc(*texto, nueva);
        if (!ampliado) return -1;
        *texto = ampliado;
        *capacidad = nueva;
    }
    memcpy(*texto + *largo, linea, n);
    *largo += n;
    (*texto)[(*largo)++] = '\n';
    return 0;
}

/**
 * @brief Crea un archivo en modo texto. El usuario escribe lineas hasta '$'
 *        (o Ctrl+D). Ctrl+C cancela sin tocar el archivo: el texto se
 *        guarda en memoria y se escribe hasta el final.
 */
void joinWrite(const char *filePath) {
    if (filePath == NULL || filePath[0] == '\0') {
        mostrarError("join>", "faltan argumentos");
        mostrarUso("join> <archivo>");
        return;
    }
    struct stat info;
    int existe = stat(filePath, &info) == 0;
    if (existe && S_ISDIR(info.st_mode)) {
        mostrarError("join>", "'%s' es un directorio", filePath);
        return;
    }
    if (existe)
        mostrarAviso("'%s' ya existe y se sobrescribira al guardar", filePath);

    printf("%sEscriba el contenido. '$' guarda, Ctrl+D guarda, Ctrl+C cancela.%s\n",
           colorSalida(ANSI_TENUE), colorSalida(ANSI_RESET));

    char *texto = NULL;
    size_t largo = 0, capacidad = 0;
    char line[BUFFER_SIZE];
    int guardar = 1;

    while (1) {
        ResultadoLectura leida = editorLeerLinea("> ", line, sizeof(line), 0);
        if (leida == LECTURA_CANCELADA) { guardar = 0; break; }
        if (leida == LECTURA_FIN_ENTRADA) break;

        /* Buscar '$': lo que este antes tambien se guarda */
        char *dollarSign = strchr(line, '$');
        size_t n = dollarSign ? (size_t)(dollarSign - line) : strlen(line);
        if ((dollarSign == NULL || n > 0) &&
            agregarLinea(&texto, &largo, &capacidad, line, n) != 0) {
            mostrarError("join>", "memoria insuficiente");
            guardar = 0;
            break;
        }
        if (dollarSign != NULL) break;
    }

    if (!guardar) {
        free(texto);
        mostrarAviso("captura cancelada; '%s' no se modifico", filePath);
        return;
    }

    int fd = open(filePath, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd == -1) {
        mostrarError("join>", "no se pudo crear '%s': %s", filePath, describirErrorSistema(errno));
        free(texto);
        return;
    }
    if (largo > 0 && write(fd, texto, largo) != (ssize_t)largo)
        mostrarError("join>", "fallo al escribir en '%s'", filePath);
    else
        mostrarExito("'%s' guardado (%zu bytes)", filePath, largo);
    close(fd);
    free(texto);
}
