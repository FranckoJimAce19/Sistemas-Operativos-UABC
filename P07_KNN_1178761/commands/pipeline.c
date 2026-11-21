/**
 * @file  pipeline.c
 * @brief Ejecucion de comandos externos y tuberias: un hijo por comando,
 *        conectados con pipe() + dup2().
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../main.h"
#include "pipeline.h"

/** @brief Cierra los dos extremos de las primeras n pipas. */
static void cerrarPipas(int fds[][2], int n) {
    for (int j = 0; j < n; j++) {
        close(fds[j][0]);
        close(fds[j][1]);
    }
}

int runPipeline(const char *line) {
    Pipeline p;
    int fds[MAX_CMDS - 1][2];
    char buffer[INPUT_SIZE];

    snprintf(buffer, sizeof(buffer), "%s", line);
    if (parser(buffer, &p) != 0)
        return -1;

    int nPipas = p.ncommands - 1;
    for (int i = 0; i < nPipas; i++) {
        if (pipe(fds[i]) != 0) {
            perror("pipe");
            cerrarPipas(fds, i);
            return -1;
        }
    }

    int hijos = 0;
    for (int i = 0; i < p.ncommands; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            break;
        }
        if (pid == 0) {
            signal(SIGINT, SIG_DFL);     /* Ctrl+C si termina al hijo */
            if (i > 0)      dup2(fds[i - 1][0], STDIN_FILENO);   /* entrada */
            if (i < nPipas) dup2(fds[i][1], STDOUT_FILENO);      /* salida  */
            cerrarPipas(fds, nPipas);

            if (isBuiltin(p.args[i][0])) {
                runBuiltin(p.args[i]);
                fflush(NULL);
                _exit(0);
            }
            execvp(p.args[i][0], p.args[i]);
            fprintf(stderr, "%s: comando no encontrado\n", p.args[i][0]);
            _exit(127);
        }
        hijos++;
    }

    cerrarPipas(fds, nPipas);

    /* Esperar a todos los hijos; si Ctrl+C interrumpe wait, reintentar */
    while (hijos > 0) {
        if (wait(NULL) >= 0) hijos--;
        else if (errno != EINTR) break;
    }
    return 0;
}
