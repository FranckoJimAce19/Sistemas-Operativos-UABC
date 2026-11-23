/**
 * @file  historial.c
 * @brief Historial de comandos del interprete.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "historial.h"
#include "validacion.h"
#include "terminal.h"

static char *comandos[MAX_HISTORIAL];   /* comandos guardados (0 = mas antiguo) */
static int cantidad = 0;
static char rutaArchivo[1024] = "";     /* vacia = no se guarda en archivo */

/** @brief Agrega a memoria; si esta lleno descarta el mas antiguo. */
static void guardarEnMemoria(const char *linea) {
    char *copia = strdup(linea);
    if (!copia) return;
    if (cantidad == MAX_HISTORIAL) {
        free(comandos[0]);
        memmove(&comandos[0], &comandos[1], (MAX_HISTORIAL - 1) * sizeof(char *));
        cantidad--;
    }
    comandos[cantidad++] = copia;
}

/** @brief Reescribe el archivo con lo que hay en memoria. */
static void reescribirArchivo(void) {
    if (rutaArchivo[0] == '\0') return;
    FILE *f = fopen(rutaArchivo, "w");
    if (!f) return;
    for (int i = 0; i < cantidad; i++) fprintf(f, "%s\n", comandos[i]);
    fclose(f);
}

void historialInicializar(int usarArchivo) {
    const char *home = getenv("HOME");
    if (!usarArchivo || !home) return;
    snprintf(rutaArchivo, sizeof(rutaArchivo), "%s/%s", home, ARCHIVO_HISTORIAL);

    FILE *f = fopen(rutaArchivo, "r");
    if (!f) return;
    char linea[2048];
    int leidas = 0;
    while (fgets(linea, sizeof(linea), f)) {
        linea[strcspn(linea, "\r\n")] = '\0';
        if (linea[0] != '\0') { guardarEnMemoria(linea); leidas++; }
    }
    fclose(f);
    if (leidas > MAX_HISTORIAL) reescribirArchivo();   /* recortar el archivo */
}

void historialAgregar(const char *linea) {
    if (!linea || linea[0] == '\0') return;
    if (cantidad > 0 && strcmp(comandos[cantidad - 1], linea) == 0) return;
    guardarEnMemoria(linea);

    /* se agrega al archivo de inmediato para no perderlo si se cierra la terminal */
    if (rutaArchivo[0] != '\0') {
        FILE *f = fopen(rutaArchivo, "a");
        if (f) { fprintf(f, "%s\n", linea); fclose(f); }
    }
}

int historialCantidad(void) { return cantidad; }

const char *historialObtener(int i) {
    return (i >= 0 && i < cantidad) ? comandos[i] : NULL;
}

int historialExpandir(const char *linea, char *destino, int tamano) {
    while (*linea == ' ' || *linea == '\t') linea++;
    if (linea[0] != '!' || linea[1] == '\0' || linea[1] == ' ') return 0;

    /* la referencia llega hasta el primer espacio; lo demas se conserva */
    char referencia[256];
    int largo = (int)strcspn(linea + 1, " \t");
    if (largo >= (int)sizeof(referencia)) largo = (int)sizeof(referencia) - 1;
    memcpy(referencia, linea + 1, (size_t)largo);
    referencia[largo] = '\0';

    const char *encontrado = NULL;
    char *fin;
    long numero = strtol(referencia, &fin, 10);
    if (strcmp(referencia, "!") == 0) {                       /* !! */
        encontrado = historialObtener(cantidad - 1);
    } else if (fin != referencia && *fin == '\0') {           /* !n  o  !-n */
        encontrado = numero < 0 ? historialObtener(cantidad + (int)numero)
                                : historialObtener((int)numero - 1);
    } else {                                                  /* !texto */
        for (int i = cantidad - 1; i >= 0 && !encontrado; i--)
            if (strncmp(comandos[i], referencia, (size_t)largo) == 0) encontrado = comandos[i];
    }

    if (!encontrado) {
        mostrarError(NULL, "!%s: no existe en el historial", referencia);
        return -1;
    }
    snprintf(destino, (size_t)tamano, "%s%s", encontrado, linea + 1 + largo);
    return 1;
}

void historialMostrar(char **args) {
    int desde = 0;

    if (args[1] && strcmp(args[1], "-c") == 0) {
        historialLiberar();
        reescribirArchivo();
        mostrarExito("historial borrado");
        return;
    }
    if (args[1]) {
        int ultimos;
        if (leerEntero("history", "numero de entradas", args[1], 1, MAX_HISTORIAL, &ultimos) != 0) {
            mostrarUso("history [n | -c]");
            return;
        }
        if (ultimos < cantidad) desde = cantidad - ultimos;
    }
    if (cantidad == 0) {
        printf("El historial esta vacio.\n");
        return;
    }
    for (int i = desde; i < cantidad; i++)
        printf("%s%5d%s  %s\n", colorSalida(ANSI_TENUE), i + 1, colorSalida(ANSI_RESET),
               comandos[i]);
}

void historialLiberar(void) {
    for (int i = 0; i < cantidad; i++) free(comandos[i]);
    cantidad = 0;
}
