#define _GNU_SOURCE
/**
 * @file  main.c
 * @brief Bucle principal del interprete de comandos — Practica 2 SO.
 *
 * Comandos:
 *   join  [archivo]          — Muestra el contenido de un archivo.
 *   join> [archivo]          — Crea un archivo en modo texto (termina con $).
 *   copy  [origen] [destino] — Copia un archivo a otro destino.
 *   del   [archivo]          — Elimina un archivo.
 *   info                     — Muestra los comandos disponibles.
 *   cd   [directorio]        — Cambia el directorio de trabajo.
 *   sdir                     — Muestra el directorio de trabajo actual.
 *   clear                    — Limpia la pantalla.
 *   exit                     — Termina el interprete.
 */
#include "main.h"

/** Directorio de trabajo global */
static char cwd[INPUT_SIZE];

/**
 * @brief Muestra el prompt: so@interprete:~>>>
 */
static void showPrompt(void) {
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        strcpy(cwd, "?");

    const char *home = getenv("HOME");
    char path[INPUT_SIZE];

    if (home && strncmp(cwd, home, strlen(home)) == 0)
        snprintf(path, sizeof(path), "~%s", cwd + strlen(home));
    else
        strncpy(path, cwd, sizeof(path) - 1);

    printf(COLOR_GREEN "so" COLOR_WHITE "@" COLOR_RED "interprete" COLOR_WHITE ":" COLOR_BLUE "%s" COLOR_WHITE " >>> " COLOR_RESET, path);
    fflush(stdout);
}

/**
 * @brief Manejador de senales — Ctrl+C no sale del interprete.
 */
static void sigintHandler(int sig) {
    (void)sig;
    printf("\n");
    showPrompt();
}

/**
 * @brief Punto de entrada del interprete.
 */
int main(void) {
    char input[INPUT_SIZE];

    /* Configurar manejador de Ctrl+C */
    struct sigaction sa = { .sa_handler = sigintHandler };
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

    /* Obtener directorio inicial */
    getcwd(cwd, sizeof(cwd));

    printf(COLOR_GREEN "so@interprete" COLOR_RESET ": Intérprete personalizado. Comando 'info' para más información.\n");

    /* Bucle principal */
    while (1) {
        showPrompt();

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break;
        }

        /* Eliminar salto de linea */
        input[strcspn(input, "\n")] = '\0';
        if (input[0] == '\0')
            continue;

        /* Tokenizar entrada */
        char *command = strtok(input, " ");
        char *arg1    = strtok(NULL, " ");
        char *arg2    = strtok(NULL, " ");

        /* Identificar y ejecutar comando */
        if (strcmp(command, "exit") == 0) {
            break;
        } else if (strcmp(command, "info") == 0) {
            info();
        } else if (strcmp(command, "cd") == 0) {
            cd(arg1);
        } else if (strcmp(command, "sdir") == 0) {
            sdir();
        } else if (strcmp(command, "clear") == 0) {
            clearScreen();
        } else if (strcmp(command, "join>") == 0) {
            joinWrite(arg1);
        } else if (strcmp(command, "join") == 0) {
            joinRead(arg1);
        } else if (strcmp(command, "copy") == 0) {
            copy(arg1, arg2);
        } else if (strcmp(command, "del") == 0) {
            del(arg1);
        } else {
            printf(COLOR_RED "%s" COLOR_RESET ": comando no encontrado\n", command);
        }
    }

    return 0;
}
