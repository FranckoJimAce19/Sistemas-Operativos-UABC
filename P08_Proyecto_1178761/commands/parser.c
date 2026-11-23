/**
 * @file  parser.c
 * @brief Parser de la linea de comandos: separa por | y por espacios,
 *        con comillas, escapes y expansion de ~.
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "../utilidades/terminal.h"

/** Estado mientras se recorre la linea. */
typedef struct {
    Pipeline *p;
    size_t usado;        /* bytes ocupados en p->texto */
    int nargs;           /* argumentos del comando actual */
    int abierto;         /* 1 si se esta armando un argumento */
    int error;           /* 1 si se excedio algun limite */
} Estado;

/** @brief Agrega un caracter al argumento actual (lo abre si hace falta). */
static void agregar(Estado *e, char c) {
    if (e->error) return;
    if (e->usado + 2 > sizeof(e->p->texto) || (!e->abierto && e->nargs >= MAX_ARGS - 1)) {
        e->error = 1;
        return;
    }
    if (!e->abierto) {
        e->p->args[e->p->ncommands][e->nargs++] = &e->p->texto[e->usado];
        e->abierto = 1;
    }
    e->p->texto[e->usado++] = c;
}

/** @brief Termina el argumento actual. */
static void cerrar(Estado *e) {
    if (!e->abierto) return;
    e->p->texto[e->usado++] = '\0';
    e->abierto = 0;
}

/** @brief Abre un argumento aunque quede vacio (por ejemplo "" o ''). */
static void abrirVacio(Estado *e) {
    if (e->abierto || e->error) return;
    agregar(e, '\0');
    if (!e->error) e->usado--;   /* el '\0' se sobrescribe con el contenido */
}

/** @brief 1 si c termina un argumento sin comillas. */
static int esDelimitador(char c) {
    return c == '\0' || c == '|' || isspace((unsigned char)c);
}

/** @brief 1 si el unico argumento del comando actual es exactamente "join". */
static int esSoloJoin(const Estado *e) {
    if (e->nargs != 1) return 0;
    const char *inicio = e->p->args[e->p->ncommands][0];
    /* si sigue abierto todavia no tiene '\0': se compara por longitud */
    size_t largo = e->abierto ? (size_t)(&e->p->texto[e->usado] - inicio) : strlen(inicio);
    return largo == 4 && strncmp(inicio, "join", 4) == 0;
}

/** @brief Cierra el comando actual. @return 0 si ok, -1 si estaba vacio. */
static int terminarComando(Estado *e) {
    cerrar(e);
    if (e->nargs == 0) {
        mostrarError(NULL, "sintaxis: tuberia incompleta (falta un comando junto a '|')");
        return -1;
    }
    if (e->p->args[e->p->ncommands][0][0] == '\0') {    /* '' o "" como comando */
        mostrarError(NULL, "comando vacio");
        return -1;
    }
    e->p->args[e->p->ncommands][e->nargs] = NULL;
    e->p->ncommands++;
    e->nargs = 0;
    return 0;
}

int parser(const char *line, Pipeline *p) {
    Estado e = { .p = p };
    char comilla = '\0';

    p->ncommands = 0;
    memset(p->args, 0, sizeof(p->args));

    for (const char *c = line; *c != '\0' && !e.error; c++) {
        if (comilla == '\'') {                      /* 'literal' */
            if (*c == '\'') comilla = '\0';
            else agregar(&e, *c);
            continue;
        }
        if (comilla == '"') {                       /* "con \" y \\" */
            if (*c == '"') comilla = '\0';
            else if (*c == '\\' && (c[1] == '"' || c[1] == '\\')) agregar(&e, *++c);
            else agregar(&e, *c);
            continue;
        }

        if (isspace((unsigned char)*c)) {
            cerrar(&e);
        } else if (*c == '|') {
            if (terminarComando(&e) != 0) return -1;
            if (p->ncommands == MAX_CMDS) {
                mostrarError(NULL, "una tuberia admite como maximo %d comandos", MAX_CMDS);
                return -1;
            }
        } else if (*c == '\'' || *c == '"') {
            abrirVacio(&e);
            comilla = *c;
        } else if (*c == '\\') {
            if (c[1] != '\0') agregar(&e, *++c);
        } else if (*c == '~' && !e.abierto && (c[1] == '/' || esDelimitador(c[1]))) {
            const char *home = getenv("HOME");
            for (const char *h = home ? home : "~"; *h; h++) agregar(&e, *h);
        } else if (*c == '>' && esSoloJoin(&e)) {
            /* "join>" o "join >": el '>' es parte del nombre del comando */
            if (!e.abierto) {                       /* "join >": reabrir "join" */
                e.usado--;
                e.abierto = 1;
            }
            agregar(&e, '>');
            cerrar(&e);                             /* "join>archivo" -> "join>" "archivo" */
        } else if (strchr("<>&;", *c) != NULL) {
            mostrarError(NULL, "el operador '%c' no esta soportado (use comillas para "
                         "usarlo como texto)", *c);
            return -1;
        } else {
            agregar(&e, *c);
        }
    }

    if (e.error) {
        mostrarError(NULL, "linea demasiado larga o con mas de %d argumentos", MAX_ARGS - 1);
        return -1;
    }
    if (comilla != '\0') {
        mostrarError(NULL, "sintaxis: falta cerrar las comillas %c", comilla);
        return -1;
    }
    cerrar(&e);
    if (e.nargs == 0 && p->ncommands == 0) {
        mostrarError(NULL, "comando vacio");
        return -1;
    }
    return terminarComando(&e);
}
