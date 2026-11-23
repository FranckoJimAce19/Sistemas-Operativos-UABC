/**
 * @file  parser.h
 * @brief Parser de la linea de comandos: separa por | y por espacios.
 *
 *  - 'texto' y "texto" agrupan espacios y '|' en un solo argumento.
 *  - \x toma el caracter x literal (fuera de comillas).
 *  - ~ y ~/ruta al inicio de un argumento se expanden a $HOME.
 *  - Los operadores < > & ; no estan soportados (entre comillas si).
 *    Excepcion: "join>" / "join >" es el comando para escribir archivos.
 */
#ifndef PARSER_H
#define PARSER_H

#define MAX_ARGS   32      /* argumentos por comando (incluye el NULL final) */
#define MAX_CMDS   16       /* procesos maximos en una tuberia */
#define MAX_TEXTO  8192    /* bytes para guardar el texto de los argumentos */

typedef struct {
    int ncommands;
    char *args[MAX_CMDS][MAX_ARGS];
    char texto[MAX_TEXTO];   /* aqui apuntan los args (line no se modifica) */
} Pipeline;

/**
 * @brief Divide line en comandos y argumentos.
 * @return 0 si ok, -1 si hay error de sintaxis (ya reportado).
 */
int parser(const char *line, Pipeline *p);

#endif /* PARSER_H */
