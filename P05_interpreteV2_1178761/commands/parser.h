#ifndef PARSER_H
#define PARSER_H

#define MAX_ARGS 32
#define MAX_CMDS 64

typedef struct {
    int ncommands;
    char *args[MAX_CMDS][MAX_ARGS];
} Pipeline;

int parser(char *line, Pipeline *p);

#endif