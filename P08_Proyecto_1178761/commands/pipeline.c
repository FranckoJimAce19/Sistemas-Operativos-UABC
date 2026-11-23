/**
 * @file  pipeline.c
 * @brief Ejecucion de comandos externos y tuberias: un hijo por comando,
 *        conectados con pipe() + dup2().
 */
#define _GNU_SOURCE   /* strsignal */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "../main.h"
#include "pipeline.h"
#include "../utilidades/terminal.h"

/** @brief Cierra los dos extremos de las primeras n pipas. */
static void cerrarPipas(int fds[][2], int n) {
    for (int j = 0; j < n; j++) {
        close(fds[j][0]);
        close(fds[j][1]);
    }
}

int runPipeline(const char *line) {
    static Pipeline p;
    int fds[MAX_CMDS - 1][2];

    if (parser(line, &p) != 0)
        return -1;

    /* cd, mkprocess, alloc... cambian el estado del interprete: dentro de
       una tuberia correrian en un hijo y no tendrian efecto */
    for (int i = 0; i < p.ncommands; i++) {
        if (p.ncommands > 1 && builtinSoloPrincipal(p.args[i][0])) {
            mostrarError(p.args[i][0], "no puede usarse dentro de una tuberia "
                         "(modifica el estado del interprete)");
            return 1;
        }
    }

    int nPipas = p.ncommands - 1;
    for (int i = 0; i < nPipas; i++) {
        if (pipe(fds[i]) != 0) {
            mostrarError(NULL, "no se pudo crear la tuberia: %s", describirErrorSistema(errno));
            cerrarPipas(fds, i);
            return 1;
        }
    }

    fflush(NULL);   /* que los hijos no hereden y repitan texto pendiente */

    pid_t hijos[MAX_CMDS];
    int numHijos = 0;
    for (int i = 0; i < p.ncommands; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            mostrarError(NULL, "no se pudo crear el proceso: %s", describirErrorSistema(errno));
            break;
        }
        if (pid == 0) {
            signal(SIGINT, SIG_DFL);     /* Ctrl+C y Ctrl+\ si terminan al hijo */
            signal(SIGQUIT, SIG_DFL);
            if (i > 0)      dup2(fds[i - 1][0], STDIN_FILENO);   /* entrada */
            if (i < nPipas) dup2(fds[i][1], STDOUT_FILENO);      /* salida  */
            cerrarPipas(fds, nPipas);

            if (isBuiltin(p.args[i][0])) {
                reiniciarErrorComando();
                runBuiltin(p.args[i]);
                fflush(NULL);
                _exit(huboErrorComando() ? 1 : 0);
            }
            execvp(p.args[i][0], p.args[i]);
            struct stat info;
            if (errno == EACCES && stat(p.args[i][0], &info) == 0 && S_ISDIR(info.st_mode)) {
                mostrarError(p.args[i][0], "es un directorio");
                _exit(126);
            }
            if (errno == ENOENT) {
                mostrarError(p.args[i][0], "comando no encontrado");
                _exit(127);
            }
            mostrarError(p.args[i][0], "no se pudo ejecutar: %s", describirErrorSistema(errno));
            _exit(126);
        }
        hijos[numHijos++] = pid;
    }

    cerrarPipas(fds, nPipas);

    /* Esperar a todos los hijos (si Ctrl+C interrumpe wait, reintentar) y
       quedarse con el estado del ultimo comando de la tuberia */
    int estadoFinal = numHijos == p.ncommands ? 0 : 1;
    for (int i = 0; i < numHijos; i++) {
        int estado = 0;
        while (waitpid(hijos[i], &estado, 0) < 0 && errno == EINTR) { }
        if (WIFSIGNALED(estado) && WTERMSIG(estado) != SIGINT && WTERMSIG(estado) != SIGPIPE)
            fprintf(stderr, "%s%s%s\n", colorError(ANSI_AMARILLO), strsignal(WTERMSIG(estado)),
                    colorError(ANSI_RESET));
        if (i == p.ncommands - 1)
            estadoFinal = WIFEXITED(estado) ? WEXITSTATUS(estado) : 128 + WTERMSIG(estado);
    }
    return estadoFinal;
}
