/**
 * @file  cd.c
 * @brief Comando cd — cambia el directorio de trabajo del interprete.
 *
 * Maneja los siguientes casos:
 *   - Sin argumentos: cambia al HOME del usuario.
 *   - "~": cambia al HOME del usuario.
 *   - "~/ruta": cambia a HOME + "/ruta".
 *   - "-": regresa al directorio anterior.
 *   - "directorio": cambia al directorio especificado.
 */
#include <stdio.h>
#include <errno.h>
#include "../utilidades/terminal.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include "cd.h"

/**
 * @brief Obtiene el directorio HOME del usuario.
 *        Primero intenta con getenv("HOME"), luego con getpwuid().
 * @return Ruta del directorio HOME.
 */
static const char *getHomeDirectory(void) {
    const char *homeDir = getenv("HOME");
    if (!homeDir) {
        struct passwd *passwdEntry = getpwuid(getuid());
        if (passwdEntry) homeDir = passwdEntry->pw_dir;
    }
    return homeDir;
}

/**
 * @brief Cambia el directorio de trabajo.
 */
void cd(const char *path) {
    static char directorioAnterior[4096] = "";   /* para "cd -" */
    const char *homeDir = getHomeDirectory();
    char fullPath[4096];
    char actual[4096];
    const char *destino = path;

    if (getcwd(actual, sizeof(actual)) == NULL) actual[0] = '\0';

    if (path != NULL && strcmp(path, "-") == 0) {
        if (directorioAnterior[0] == '\0') {
            mostrarError("cd", "no hay directorio anterior");
            return;
        }
        snprintf(fullPath, sizeof(fullPath), "%s", directorioAnterior);
        destino = fullPath;
    }

    if (path == NULL || path[0] == '\0' || strcmp(path, "~") == 0 ||
        strncmp(path, "~/", 2) == 0) {
        if (homeDir == NULL) {
            mostrarError("cd", "no se pudo determinar el directorio HOME");
            return;
        }
        if (path != NULL && strncmp(path, "~/", 2) == 0) {
            /* Expandir "~/ruta" a HOME/ruta */
            snprintf(fullPath, sizeof(fullPath), "%s/%s", homeDir, path + 2);
            destino = fullPath;
        } else {
            destino = homeDir;   /* sin argumentos o "~" */
        }
    }

    if (chdir(destino) != 0) {
        if (errno == ENOENT)
            mostrarError("cd", "'%s': no existe el directorio", destino);
        else if (errno == ENOTDIR)
            mostrarError("cd", "'%s': no es un directorio", destino);
        else
            mostrarError("cd", "'%s': %s", destino, describirErrorSistema(errno));
        return;
    }

    if (path != NULL && strcmp(path, "-") == 0)
        printf("%s\n", destino);                   /* como en bash: muestra a donde fue */
    snprintf(directorioAnterior, sizeof(directorioAnterior), "%s", actual);
}
