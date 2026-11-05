#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include "parser.h"

int parser(char *line, Pipeline *p) {
    char *saveptr1, *saveptr2;
    char *segmento, *ficha;

    p->ncommands = 0;
    memset(p->args, 0, sizeof(p->args));

    //Verificar que la linea no empiece ni termine con '|'
    char *ini = line;
    while (*ini == ' ' || *ini == '\t') ini++;
    if (*ini == '\0') {
        fprintf(stderr, "Error: comando vacio\n");
        return -1;
    }
    if (*ini == '|') {
        fprintf(stderr, "Error: tuberia incompleta\n");
        return -1;
    }
    char *fin = ini + strlen(ini) - 1;
    while (fin > ini && (*fin == ' ' || *fin == '\t')) fin--;
    if (*fin == '|') {
        fprintf(stderr, "Error: tuberia incompleta\n");
        return -1;
    }

    //Dividir el comando por "|"
    for (segmento = strtok_r(line, "|", &saveptr1);
         segmento != NULL;
         segmento = strtok_r(NULL, "|", &saveptr1)) {
            if (p->ncommands == MAX_CMDS) {
                fprintf(stderr, "Error: demasiados comandos\n");
                return -1;
            }
            // Dividir comando en parametros y comando
            int nargs = 0;
            for (ficha = strtok_r(segmento, " \t", &saveptr2);
                ficha != NULL;
                ficha = strtok_r(NULL, " \t", &saveptr2)) {
                    if (nargs == MAX_ARGS - 1) {
                        fprintf(stderr, "Error: demasiados argumentos\n");
                        return -1;
                    }
                    p->args[p->ncommands][nargs++] = ficha;
                 }
            //Segmento incompleto, pipa incompleta
            if (nargs == 0) {
                fprintf(stderr, "Error: tuberia incompleta\n");
                return -1;
            }
            p->args[p->ncommands][nargs] = NULL;
            p->ncommands++;
         }
    if (p->ncommands == 0) {
        fprintf(stderr, "Error: comando vacio\n");
        return -1;
    }
    return 0;
}