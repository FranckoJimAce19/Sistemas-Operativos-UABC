/**
 * @file  terminal.c
 * @brief Implementacion de los mensajes y formatos de salida del interprete.
 */
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "terminal.h"

const char *colorSalida(const char *codigoAnsi) {
    return isatty(STDOUT_FILENO) ? codigoAnsi : "";
}

const char *colorError(const char *codigoAnsi) {
    return isatty(STDERR_FILENO) ? codigoAnsi : "";
}

/** 1 si el comando actual reporto algun error. */
static int errorEnComando = 0;

int huboErrorComando(void)     { return errorEnComando; }
void reiniciarErrorComando(void) { errorEnComando = 0; }
void marcarErrorComando(void)  { errorEnComando = 1; }

void mostrarError(const char *nombreComando, const char *formato, ...) {
    va_list argumentosVariables;

    errorEnComando = 1;
    /* stdout puede tener texto pendiente: se vacia para conservar el orden */
    fflush(stdout);
    fprintf(stderr, "%s", colorError(ANSI_ROJO));
    if (nombreComando != NULL)
        fprintf(stderr, "%s: ", nombreComando);
    fprintf(stderr, "error:%s ", colorError(ANSI_RESET));

    va_start(argumentosVariables, formato);
    vfprintf(stderr, formato, argumentosVariables);
    va_end(argumentosVariables);
    fputc('\n', stderr);
}

void mostrarAviso(const char *formato, ...) {
    va_list argumentosVariables;

    printf("%saviso:%s ", colorSalida(ANSI_AMARILLO), colorSalida(ANSI_RESET));
    va_start(argumentosVariables, formato);
    vprintf(formato, argumentosVariables);
    va_end(argumentosVariables);
    putchar('\n');
}

void mostrarExito(const char *formato, ...) {
    va_list argumentosVariables;

    printf("%s✔%s ", colorSalida(ANSI_VERDE), colorSalida(ANSI_RESET));
    va_start(argumentosVariables, formato);
    vprintf(formato, argumentosVariables);
    va_end(argumentosVariables);
    putchar('\n');
}

void mostrarUso(const char *sintaxis) {
    fprintf(stderr, "%suso:%s %s\n", colorError(ANSI_AMARILLO),
            colorError(ANSI_RESET), sintaxis);
}

void imprimirLineaSeparadora(const char *caracter, int ancho) {
    /* En terminales angostas la linea no debe partirse en dos renglones */
    if (isatty(STDOUT_FILENO) && ancho > obtenerAnchoTerminal())
        ancho = obtenerAnchoTerminal();
    for (int columna = 0; columna < ancho; columna++)
        fputs(caracter, stdout);
    putchar('\n');
}

void imprimirTituloPrincipal(const char *titulo) {
    printf("\n%s", colorSalida(ANSI_CIAN));
    imprimirLineaSeparadora("═", ANCHO_REPORTE);
    printf("  %s\n", titulo);
    imprimirLineaSeparadora("═", ANCHO_REPORTE);
    printf("%s", colorSalida(ANSI_RESET));
}

void imprimirSubtitulo(const char *subtitulo) {
    printf("\n%s%s%s\n", colorSalida(ANSI_NEGRITA), subtitulo, colorSalida(ANSI_RESET));
    imprimirLineaSeparadora("─", ANCHO_REPORTE);
}

int obtenerAnchoTerminal(void) {
    struct winsize tamanoVentana;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &tamanoVentana) == 0 && tamanoVentana.ws_col > 0)
        return tamanoVentana.ws_col;
    return 80;
}

const char *describirErrorSistema(int numeroError) {
    switch (numeroError) {
    case ENOENT:       return "no existe el archivo o directorio";
    case EACCES:       return "permiso denegado";
    case EPERM:        return "operacion no permitida";
    case EISDIR:       return "es un directorio";
    case ENOTDIR:      return "no es un directorio";
    case EEXIST:       return "el archivo ya existe";
    case ENOSPC:       return "no queda espacio en el dispositivo";
    case EROFS:        return "sistema de archivos de solo lectura";
    case ENAMETOOLONG: return "nombre de archivo demasiado largo";
    case ELOOP:        return "demasiados enlaces simbolicos";
    case EBUSY:        return "el recurso esta ocupado";
    case EMFILE:       return "demasiados archivos abiertos";
    case ENOMEM:       return "memoria insuficiente";
    case ENOEXEC:      return "formato de ejecutable no valido";
    case EIO:          return "error de entrada/salida";
    default:           return strerror(numeroError);
    }
}
