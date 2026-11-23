/**
 * @file  info.c
 * @brief Comando info — muestra todos los comandos disponibles del interprete.
 *        Lee la ayuda desde commands/info.txt para facilitar actualizaciones.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include "info.h"
#include "../utilidades/terminal.h"

#define ANCHO_SINTAXIS 38   /* columna donde inicia la descripcion */

/**
 * @brief Imprime una linea de info.txt segun su formato
 *        ([Titulo], "sintaxis | descripcion", "> nota" o texto libre).
 */
static void imprimirLineaAyuda(char *linea) {
    linea[strcspn(linea, "\r\n")] = '\0';

    if (linea[0] == '#') return;                       /* comentario */

    if (linea[0] == '[') {                             /* categoria */
        char *cierre = strchr(linea, ']');
        if (cierre) *cierre = '\0';
        printf("\n%s%s%s\n", colorSalida(ANSI_AMARILLO), linea + 1, colorSalida(ANSI_RESET));
        return;
    }
    if (linea[0] == '>') {                             /* nota */
        printf("\n%s%s%s\n", colorSalida(ANSI_TENUE), linea + 2, colorSalida(ANSI_RESET));
        return;
    }

    /* se usa el ultimo " | " porque la sintaxis puede contener '|' (tuberias) */
    char *separador = NULL;
    for (char *busqueda = strstr(linea, " | "); busqueda; busqueda = strstr(busqueda + 1, " | "))
        separador = busqueda;
    if (separador == NULL || linea[0] == ' ') {        /* texto libre */
        if (linea[0] != '\0') printf("%s\n", linea);
        return;
    }

    /* comando: quitar los espacios de relleno de la sintaxis */
    *separador = '\0';
    char *fin = separador;
    while (fin > linea && fin[-1] == ' ') *--fin = '\0';
    printf("  %s%-*s%s %s\n", colorSalida(ANSI_NEGRITA), ANCHO_SINTAXIS, linea,
           colorSalida(ANSI_RESET), separador + 3);
}

/**
 * @brief Lee y muestra el archivo de ayuda usando /proc/self/exe
 *        para encontrar la ruta relativa al ejecutable.
 */
void info(void) {
    char exe[512];
    char path[1024] = "commands/info.txt";
    ssize_t len = readlink("/proc/self/exe", exe, sizeof(exe) - 1);
    if (len != -1) {
        exe[len] = '\0';
        snprintf(path, sizeof(path), "%s/commands/info.txt", dirname(exe));
    }

    FILE *archivo = fopen(path, "r");
    if (archivo == NULL) archivo = fopen("commands/info.txt", "r");   /* desde el directorio actual */
    if (archivo == NULL) {
        mostrarError("info", "no se pudo abrir '%s'", path);
        return;
    }

    imprimirTituloPrincipal("Interprete de comandos  |  Ayuda");
    char linea[512];
    while (fgets(linea, sizeof(linea), archivo))
        imprimirLineaAyuda(linea);
    printf("\n");
    fclose(archivo);
}
