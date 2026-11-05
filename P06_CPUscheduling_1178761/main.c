#define _GNU_SOURCE
/**
 * @file  main.c
 * @brief Bucle principal del interprete de comandos — Practica 6 SO.
 */
#include "main.h"

/** Directorio de trabajo global */
static char cwd[INPUT_SIZE];

/** Lista de procesos global para CPU Scheduling */
ListaProcesos listaProcesosGlobal;

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

int isBuiltin(const char *cmd) {
    return strcmp(cmd, "exit")        == 0 || strcmp(cmd, "info")        == 0 ||
           strcmp(cmd, "cd")          == 0 || strcmp(cmd, "sdir")        == 0 ||
           strcmp(cmd, "clear")       == 0 || strcmp(cmd, "join>")       == 0 ||
           strcmp(cmd, "join")        == 0 || strcmp(cmd, "copy")        == 0 ||
           strcmp(cmd, "del")         == 0 ||
           strcmp(cmd, "mkprocess")   == 0 ||
           strcmp(cmd, "lstprocss")   == 0 ||
           strcmp(cmd, "fcfs")        == 0 ||
           strcmp(cmd, "sjf")         == 0 ||
           strcmp(cmd, "rr")          == 0;
}

int runBuiltin(char **args) {
    if (strcmp(args[0], "exit") == 0) return 1;
    if (strcmp(args[0], "info") == 0) { info(); return 0; }
    if (strcmp(args[0], "cd") == 0) { cd(args[1]); return 0; }
    if (strcmp(args[0], "sdir") == 0) { sdir(); return 0; }
    if (strcmp(args[0], "clear") == 0) { clearScreen(); return 0; }
    if (strcmp(args[0], "join>") == 0) { joinWrite(args[1]); return 0; }
    if (strcmp(args[0], "join") == 0) { joinRead(args[1]); return 0; }
    if (strcmp(args[0], "copy") == 0) { copy(args[1], args[2]); return 0; }
    if (strcmp(args[0], "del") == 0) { del(args[1]); return 0; }

    /* Comandos de planificacion de CPU */
    if (strcmp(args[0], "mkprocess") == 0) {
        cmdMkprocess(&listaProcesosGlobal, args);
        return 0;
    }
    if (strcmp(args[0], "lstprocss") == 0) {
        cmdLstprocss(&listaProcesosGlobal);
        return 0;
    }
    if (strcmp(args[0], "fcfs") == 0) {
        cmdFcfs(&listaProcesosGlobal);
        return 0;
    }
    if (strcmp(args[0], "sjf") == 0) {
        cmdSjf(&listaProcesosGlobal);
        return 0;
    }
    if (strcmp(args[0], "rr") == 0) {
        cmdRr(&listaProcesosGlobal);
        return 0;
    }

    return 0;
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

    /* Inicializar lista de procesos */
    listaInicializar(&listaProcesosGlobal);

    /* Obtener directorio inicial */
    getcwd(cwd, sizeof(cwd));

    printf(COLOR_GREEN "so@interprete" COLOR_RESET ": Practica 6 - Planificacion de CPU. Comando 'info' para mas informacion.\n");

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

        /* Comando sin tuberia se ejecuta en el padre si es builtin */
        if (strchr(input, '|') == NULL) {
            char temporal[INPUT_SIZE];
            snprintf(temporal, sizeof(temporal), "%s", input);
            char *command = strtok(temporal, " \t");

            if (command != NULL && isBuiltin(command)) {
                Pipeline p;
                if (parser(input, &p) != 0)
                    continue;
                if (runBuiltin(p.args[0]) == 1)
                    break;
                continue;
            }
        }

        /* Se ejecutan procesos externos o tuberias */
        runPipeline(input);
    }

    return 0;
}
