/**
 * @file  cd.c
 * @brief Comando cd — cambia el directorio de trabajo del interprete.
 *
 * Maneja los siguientes casos:
 *   - Sin argumentos: cambia al HOME del usuario.
 *   - "~": cambia al HOME del usuario.
 *   - "~/ruta": cambia a HOME + "/ruta".
 *   - "directorio": cambia al directorio especificado.
 */
#include <stdio.h>
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
    const char *homeDir = getHomeDirectory();

    if (path == NULL || path[0] == '\0') {
        /* Sin argumentos: ir al HOME */
        if (chdir(homeDir) != 0) {
            printf("cd: No se pudo cambiar al directorio HOME\n");
        }
    } else if (strcmp(path, "~") == 0 || strncmp(path, "~/", 2) == 0) {
        /* Expandir ~ al HOME del usuario */
        char fullPath[512];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", homeDir, path + 2);
        if (chdir(fullPath) != 0) {
            printf("cd: %s: No existe el directorio\n", fullPath);
        }
    } else {
        /* Directorio directo */
        if (chdir(path) != 0) {
            printf("cd: %s: No existe el directorio\n", path);
        }
    }
}
