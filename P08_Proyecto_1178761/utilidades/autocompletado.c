/**
 * @file  autocompletado.c
 * @brief Candidatos de autocompletado para la tecla TAB.
 */
#define _GNU_SOURCE
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "autocompletado.h"
#include "../main.h"

/** @brief 1 si la ruta existe y es un directorio. */
static int esDirectorio(const char *ruta) {
    struct stat info;
    return stat(ruta, &info) == 0 && S_ISDIR(info.st_mode);
}

/** Limite de candidatos para no saturar la memoria con un PATH enorme. */
#define MAXIMO_CANDIDATOS 4096
/** Prefijo de las unidades de Windows montadas en WSL: se omiten del PATH
    al autocompletar porque son lentas de recorrer y llenan la lista de .exe */
#define PREFIJO_UNIDADES_WINDOWS "/mnt/"
/** Longitud maxima de la palabra que se intenta completar. */
#define TAMANO_PALABRA_COMPLETADO 2048

/**
 * @brief Agrega una copia del candidato a la lista.
 */
static void agregarCandidato(ListaCandidatos *lista, const char *candidato) {
    if (lista->cantidad >= MAXIMO_CANDIDATOS)
        return;
    if (lista->cantidad == lista->capacidad) {
        size_t nuevaCapacidad = lista->capacidad ? lista->capacidad * 2 : 32;
        char **candidatosAmpliados = realloc(lista->candidatos, nuevaCapacidad * sizeof(char *));
        if (candidatosAmpliados == NULL)
            return;
        lista->candidatos = candidatosAmpliados;
        lista->capacidad = nuevaCapacidad;
    }
    char *copiaCandidato = strdup(candidato);
    if (copiaCandidato != NULL)
        lista->candidatos[lista->cantidad++] = copiaCandidato;
}

/** @brief Comparador para qsort de cadenas. */
static int compararCadenas(const void *primera, const void *segunda) {
    return strcmp(*(char *const *)primera, *(char *const *)segunda);
}

/**
 * @brief Ordena la lista y elimina candidatos repetidos.
 */
static void ordenarYQuitarRepetidos(ListaCandidatos *lista) {
    if (lista->cantidad < 2)
        return;
    qsort(lista->candidatos, lista->cantidad, sizeof(char *), compararCadenas);

    size_t cantidadUnicos = 1;
    for (size_t indice = 1; indice < lista->cantidad; indice++) {
        if (strcmp(lista->candidatos[indice], lista->candidatos[cantidadUnicos - 1]) == 0)
            free(lista->candidatos[indice]);
        else
            lista->candidatos[cantidadUnicos++] = lista->candidatos[indice];
    }
    lista->cantidad = cantidadUnicos;
}

/**
 * @brief Agrega comandos internos y ejecutables del PATH que empiezan con prefijo.
 */
static void agregarCandidatosComando(const char *prefijo, ListaCandidatos *lista) {
    size_t longitudPrefijo = strlen(prefijo);
    /* Comandos internos del interprete (tabla BUILTINS de main.c) */
    for (int indice = 0; nombreBuiltin(indice) != NULL; indice++)
        if (nombreBuiltin(indice)[0] != '\0' &&     /* "" = alias, no se sugiere */
            strncmp(nombreBuiltin(indice), prefijo, longitudPrefijo) == 0)
            agregarCandidato(lista, nombreBuiltin(indice));

    /* Sin prefijo se listarian miles de programas: solo comandos internos */
    const char *variablePath = getenv("PATH");
    if (longitudPrefijo == 0 || variablePath == NULL)
        return;

    char *copiaPath = strdup(variablePath);
    if (copiaPath == NULL)
        return;

    char *contextoTokens = NULL;
    for (char *directorio = strtok_r(copiaPath, ":", &contextoTokens); directorio != NULL;
         directorio = strtok_r(NULL, ":", &contextoTokens)) {
        if (strncmp(directorio, PREFIJO_UNIDADES_WINDOWS, strlen(PREFIJO_UNIDADES_WINDOWS)) == 0)
            continue;
        DIR *flujoDirectorio = opendir(directorio);
        if (flujoDirectorio == NULL)
            continue;

        struct dirent *entrada;
        while ((entrada = readdir(flujoDirectorio)) != NULL) {
            if (strncmp(entrada->d_name, prefijo, longitudPrefijo) != 0 ||
                entrada->d_type == DT_DIR)
                continue;
            char rutaCompleta[4096];
            snprintf(rutaCompleta, sizeof(rutaCompleta), "%s/%s", directorio, entrada->d_name);
            if (access(rutaCompleta, X_OK) == 0 && !esDirectorio(rutaCompleta))
                agregarCandidato(lista, entrada->d_name);
        }
        closedir(flujoDirectorio);
    }
    free(copiaPath);
}

/**
 * @brief Agrega archivos y directorios que coinciden con la palabra.
 *        "doc/ar" busca en "doc/" las entradas que empiezan con "ar".
 */
static void agregarCandidatosArchivo(const char *palabra, ListaCandidatos *lista) {
    const char *ultimaDiagonal = strrchr(palabra, '/');
    size_t longitudDirectorio = ultimaDiagonal ? (size_t)(ultimaDiagonal - palabra + 1) : 0;
    const char *prefijoNombre = palabra + longitudDirectorio;
    size_t longitudPrefijo = strlen(prefijoNombre);

    /* Directorio a leer, expandiendo "~/" al HOME */
    char directorioEscrito[4096];
    char directorioReal[4096];
    snprintf(directorioEscrito, sizeof(directorioEscrito), "%.*s", (int)longitudDirectorio, palabra);

    const char *directorioHome = getenv("HOME");
    if (longitudDirectorio == 0)
        snprintf(directorioReal, sizeof(directorioReal), ".");
    else if (strncmp(directorioEscrito, "~/", 2) == 0 && directorioHome != NULL)
        snprintf(directorioReal, sizeof(directorioReal), "%s/%s", directorioHome,
                 directorioEscrito + 2);
    else
        snprintf(directorioReal, sizeof(directorioReal), "%s", directorioEscrito);

    DIR *flujoDirectorio = opendir(directorioReal);
    if (flujoDirectorio == NULL)
        return;

    struct dirent *entrada;
    while ((entrada = readdir(flujoDirectorio)) != NULL) {
        const char *nombre = entrada->d_name;
        if (strcmp(nombre, ".") == 0 || strcmp(nombre, "..") == 0)
            continue;
        if (nombre[0] == '.' && prefijoNombre[0] != '.')
            continue;   /* ocultos solo si el usuario escribio el punto */
        if (strncmp(nombre, prefijoNombre, longitudPrefijo) != 0)
            continue;

        char rutaEntrada[8192];
        char candidato[8192];
        snprintf(rutaEntrada, sizeof(rutaEntrada), "%s/%s", directorioReal, nombre);
        snprintf(candidato, sizeof(candidato), "%s%s%s", directorioEscrito, nombre,
                 esDirectorio(rutaEntrada) ? "/" : "");
        agregarCandidato(lista, candidato);
    }
    closedir(flujoDirectorio);
}

size_t buscarCandidatosCompletado(const char *linea, size_t posicionCursor,
                                  ListaCandidatos *lista) {
    memset(lista, 0, sizeof(*lista));

    size_t inicioPalabra = posicionCursor;
    while (inicioPalabra > 0 && linea[inicioPalabra - 1] != ' ' &&
           linea[inicioPalabra - 1] != '\t' && linea[inicioPalabra - 1] != '|')
        inicioPalabra--;

    /* Es posicion de comando si antes de la palabra solo hay espacios
       hasta el inicio de la linea o hasta un '|' */
    size_t posicionAnterior = inicioPalabra;
    while (posicionAnterior > 0 &&
           (linea[posicionAnterior - 1] == ' ' || linea[posicionAnterior - 1] == '\t'))
        posicionAnterior--;
    int esPosicionComando = posicionAnterior == 0 || linea[posicionAnterior - 1] == '|';

    char palabra[TAMANO_PALABRA_COMPLETADO];
    size_t longitudPalabra = posicionCursor - inicioPalabra;
    if (longitudPalabra >= sizeof(palabra))
        return inicioPalabra;
    memcpy(palabra, linea + inicioPalabra, longitudPalabra);
    palabra[longitudPalabra] = '\0';

    if (esPosicionComando && strchr(palabra, '/') == NULL)
        agregarCandidatosComando(palabra, lista);
    else
        agregarCandidatosArchivo(palabra, lista);

    ordenarYQuitarRepetidos(lista);
    return inicioPalabra;
}

size_t calcularPrefijoComun(const ListaCandidatos *lista) {
    if (lista->cantidad == 0)
        return 0;

    size_t longitudComun = strlen(lista->candidatos[0]);
    for (size_t indice = 1; indice < lista->cantidad; indice++) {
        size_t posicion = 0;
        while (posicion < longitudComun &&
               lista->candidatos[indice][posicion] == lista->candidatos[0][posicion])
            posicion++;
        longitudComun = posicion;
    }
    return longitudComun;
}

void liberarCandidatos(ListaCandidatos *lista) {
    for (size_t indice = 0; indice < lista->cantidad; indice++)
        free(lista->candidatos[indice]);
    free(lista->candidatos);
    memset(lista, 0, sizeof(*lista));
}
