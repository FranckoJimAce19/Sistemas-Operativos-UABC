/**
 * @file  parser.h
 * @brief Parser de la linea de comandos: separa por | y por espacios.
 */
#ifndef PARSER_H
#define PARSER_H

#define MAX_ARGS 32
#define MAX_CMDS 64

typedef struct {
    int ncommands;
    char *args[MAX_CMDS][MAX_ARGS];
} Pipeline;

/**
 * @brief Divide line en comandos y argumentos (modifica line).
 * @return 0 si ok, -1 si hay error de sintaxis.
 */
int parser(char *line, Pipeline *p);

#endif /* PARSER_H */
