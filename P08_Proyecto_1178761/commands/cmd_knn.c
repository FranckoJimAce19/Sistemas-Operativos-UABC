/**
 * @file  cmd_knn.c
 * @brief Comando 'knn' del interprete. Usa el modulo knn/.
 */
#define _DEFAULT_SOURCE   /* readlink */
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "cmd_knn.h"
#include "../knn/busqueda.h"
#include "../utilidades/terminal.h"
#include "../utilidades/validacion.h"

/** @brief 1 si la ruta existe y es un directorio. */
static int esDirectorio(const char *ruta) {
    struct stat info;
    return stat(ruta, &info) == 0 && S_ISDIR(info.st_mode);
}

/**
 * @brief Carpeta "data" por defecto: la del directorio actual o, si no
 *        existe, la que esta junto al ejecutable (asi funciona tras un cd).
 */
static const char *directorioPorDefecto(char *ruta, size_t tamano) {
    char exe[2048];
    if (esDirectorio("data")) return "data";

    ssize_t largo = readlink("/proc/self/exe", exe, sizeof(exe) - 1);
    if (largo > 0) {
        exe[largo] = '\0';
        char *diagonal = strrchr(exe, '/');
        if (diagonal) *diagonal = '\0';
        snprintf(ruta, tamano, "%s/data", exe);
        if (esDirectorio(ruta)) return ruta;
    }
    return "data";
}

void cmdKnn(char **args) {
    char rutaDefecto[4096];
    const char *dir = args[1];
    const char *textoHilos = args[1] ? args[2] : NULL;
    int hilos = 0;                  /* 0 = comparar 1, 2 y 3 hilos */

    if (args[1] && args[2] && args[3]) {
        mostrarError("knn", "demasiados argumentos");
        mostrarUso("knn [directorio] [hilos]");
        return;
    }
    /* "knn 3": si el unico argumento es un numero y no una carpeta, son los hilos */
    if (args[1] && !args[2] && !esDirectorio(args[1]) &&
        strspn(args[1], "0123456789") == strlen(args[1])) {
        dir = NULL;
        textoHilos = args[1];
    }
    if (dir == NULL) dir = directorioPorDefecto(rutaDefecto, sizeof(rutaDefecto));

    if (textoHilos &&
        leerEntero("knn", "numero de hilos", textoHilos, 1, MAX_HILOS, &hilos) != 0) {
        mostrarUso("knn [directorio] [hilos]");
        return;
    }
    if (!esDirectorio(dir)) {
        mostrarError("knn", "'%s' no es un directorio con los archivos CSV del dataset", dir);
        return;
    }

    /* La busqueda tarda: se hace en un hijo para que Ctrl+C la detenga
       sin cerrar el interprete */
    fflush(stdout);
    pid_t pid = fork();
    if (pid < 0) {
        mostrarError("knn", "no se pudo crear el proceso: %s", describirErrorSistema(errno));
        return;
    }
    if (pid == 0) {
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        int resultado = busquedaKnn(dir, hilos);
        fflush(stdout);
        _exit(resultado == 0 ? 0 : 1);
    }

    int estado;
    while (waitpid(pid, &estado, 0) < 0 && errno == EINTR) { }
    if (WIFSIGNALED(estado) && WTERMSIG(estado) == SIGINT) {
        mostrarAviso("busqueda K-NN cancelada");
        marcarErrorComando();
    } else if (!WIFEXITED(estado) || WEXITSTATUS(estado) != 0) {
        marcarErrorComando();
    }
}
