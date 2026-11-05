#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../main.h"
#include "pipeline.h"

int runPipeline(const char *line) {
    Pipeline p;
    int fds[MAX_CMDS - 1][2];
    int i, j;
    char buffer[INPUT_SIZE];
    snprintf(buffer, sizeof(buffer), "%s", line);
    if (parser(buffer, &p) != 0) {
        return -1;
    }
    
    //Creando n pipas
    for (i = 0; i < p.ncommands - 1; i++) {
        if (pipe(fds[i]) != 0) {
            perror("pipe");
            for (j = 0; j < i; j++) {
                close(fds[j][0]);
                close(fds[j][1]);
            }
            return -1;
        }
    }

    //Comando por hijo
    for (i = 0; i < p.ncommands; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            for (j = 0; j < p.ncommands - 1; j++) {
                close(fds[j][0]);
                close(fds[j][1]);
            }
            return -1;
        }
        if (pid == 0) {
           //Entrada de la pipa anterior
           if (i > 0) {
            dup2(fds[i-1][0],    STDIN_FILENO);
           } 
           //Salida de la pipa siguiente
           if (i < p.ncommands - 1) {
            dup2(fds[i][1], STDOUT_FILENO);
           }
           //Cerrando los extremos de las pipas
           for (j = 0; j < p.ncommands - 1; j++) {
            close(fds[j][0]);
            close(fds[j][1]);
           }
           if (isBuiltin(p.args[i][0])){
            runBuiltin(p.args[i]);
            fflush(NULL);
            _exit(0);
           }
           //Reemplazando la imagen
           execvp(p.args[i][0], p.args[i]);
           fprintf(stderr, "%s: comando no encontrado\n", p.args[i][0]);
           _exit(127);
        }
    }
    //Cerrando los extremos de las pipas
    for (j = 0; j < p.ncommands - 1; j++) {
        close(fds[j][0]);
        close(fds[j][1]);
    }
    for (i = 0; i < p.ncommands; i++) {
        wait(NULL);
    }
    return 0;
}