/**
 * @file  main.c
 * @brief Bucle principal del interprete de comandos.
 */
#define _GNU_SOURCE
#include <errno.h>
#include <strings.h>
#include "main.h"

/** Lista de procesos global (mkprocess, lstprocess, fcfs, sjf, rr...) */
ListaProcesos listaProcesosGlobal;

/* ---------- Adaptadores: todos los builtins reciben args ---------- */
static void bInfo(char **a)       { (void)a; info(); }
static void bCd(char **a)         { cd(a[1]); }
static void bSdir(char **a)       { (void)a; sdir(); }
static void bClear(char **a)      { (void)a; clearScreen(); }
static void bJoinWrite(char **a)  { joinWrite(a[1]); }
static void bJoinRead(char **a)   { joinRead(a[1]); }
static void bCopy(char **a)       { copy(a[1], a[2]); }
static void bDel(char **a) {
    if (!a[1]) del(NULL);                          /* muestra el uso */
    for (int i = 1; a[i]; i++) del(a[i]);          /* uno o varios archivos */
}
static void bHistory(char **a)    { historialMostrar(a); }
static void bMkprocess(char **a)  { cmdMkprocess(&listaProcesosGlobal, a); }
static void bLstprocess(char **a) { (void)a; cmdLstprocess(&listaProcesosGlobal); }
static void bKbill(char **a)       { cmdKbill(&listaProcesosGlobal, a); }
static void bFcfs(char **a)       { (void)a; cmdFcfs(&listaProcesosGlobal); }
static void bSjf(char **a)        { (void)a; cmdSjf(&listaProcesosGlobal); }
static void bRr(char **a)         { (void)a; cmdRr(&listaProcesosGlobal); }
static void bAlloc(char **a)      { cmdAlloc(&listaProcesosGlobal, a); }
static void bFree(char **a)       { cmdFree(&listaProcesosGlobal, a); }
static void bMstatus(char **a)    { (void)a; cmdMstatus(); }
static void bCompact(char **a)    { (void)a; cmdCompact(&listaProcesosGlobal); }

#define SIN_LIMITE -1   /* maxArgs: cualquier cantidad de argumentos */

/**
 * Tabla de comandos internos. 'exit' se maneja aparte en runBuiltin.
 *  maxArgs:       argumentos maximos (sin contar el comando); si se pasan
 *                 mas se muestra el error y el uso.
 *  soloPrincipal: 1 si cambia el estado del interprete (no va en tuberias).
 *  esAlias:       1 si es otro nombre de un comando (no se sugiere con TAB).
 */
static const struct {
    const char *nombre;
    void (*fn)(char **args);
    int maxArgs;
    const char *uso;
    int soloPrincipal;
    int esAlias;
} BUILTINS[] = {
    {"info",    bInfo,       0, "info",                              0, 0},
    {"help",    bInfo,       0, "help",                              0, 0},
    {"cd",      bCd,         1, "cd [directorio | ~ | -]",           1, 0},
    {"sdir",    bSdir,       0, "sdir",                              0, 0},
    {"clear",   bClear,      0, "clear",                             0, 0},
    {"join>",   bJoinWrite,  1, "join> <archivo>",                   1, 0},
    {"join",    bJoinRead,   1, "join <archivo>",                    0, 0},
    {"copy",    bCopy,       2, "copy <origen> <destino>",           0, 0},
    {"del",     bDel, SIN_LIMITE, "del <archivo> [archivo ...]",     0, 0},
    {"history", bHistory,    1, "history [n | -c]",                  0, 0},
    /* Procesos y planificacion de CPU */
    {"mkprocess",  bMkprocess,  3, "mkprocess <id> <burst time> <bloques>", 1, 0},
    {"lstprocess", bLstprocess, 0, "lstprocess",                     0, 0},
    {"lstprocss",  bLstprocess, 0, "lstprocess",                     0, 1},
    {"kbill",   bKbill,      1, "kbill <id>",                        1, 0},
    {"fcfs",    bFcfs,       0, "fcfs",                              1, 0},
    {"sjf",     bSjf,        0, "sjf",                               1, 0},
    {"rr",      bRr,         0, "rr",                                1, 0},
    /* Memoria */
    {"alloc",   bAlloc,      2, "alloc <id> <first | best | worst>", 1, 0},
    {"free",    bFree,       1, "free <id>",                         1, 0},
    {"mstatus", bMstatus,    0, "mstatus",                           0, 0},
    {"compact", bCompact,    0, "compact",                           1, 0},
    /* K-NN con hilos */
    {"knn",     cmdKnn,      2, "knn [directorio] [hilos]",          0, 0},
};
#define NUM_BUILTINS (sizeof(BUILTINS) / sizeof(BUILTINS[0]))

int isBuiltin(const char *cmd) {
    if (strcasecmp(cmd, "exit") == 0) return 1;
    for (size_t i = 0; i < NUM_BUILTINS; i++)
        if (strcasecmp(cmd, BUILTINS[i].nombre) == 0) return 1;
    return 0;
}

int runBuiltin(char **args) {
    if (strcasecmp(args[0], "exit") == 0) return 1;
    for (size_t i = 0; i < NUM_BUILTINS; i++) {
        if (strcasecmp(args[0], BUILTINS[i].nombre) != 0) continue;

        int nargs = 0;
        while (args[nargs + 1]) nargs++;
        if (BUILTINS[i].maxArgs != SIN_LIMITE && nargs > BUILTINS[i].maxArgs) {
            mostrarError(BUILTINS[i].nombre, "demasiados argumentos");
            mostrarUso(BUILTINS[i].uso);
        } else {
            BUILTINS[i].fn(args);
        }
        break;
    }
    return 0;
}

int builtinSoloPrincipal(const char *cmd) {
    if (strcasecmp(cmd, "exit") == 0) return 1;
    for (size_t i = 0; i < NUM_BUILTINS; i++)
        if (strcasecmp(cmd, BUILTINS[i].nombre) == 0) return BUILTINS[i].soloPrincipal;
    return 0;
}

const char *nombreBuiltin(int i) {
    if (i == (int)NUM_BUILTINS) return "exit";
    if (i < 0 || i > (int)NUM_BUILTINS) return NULL;
    return BUILTINS[i].esAlias ? "" : BUILTINS[i].nombre;   /* "" = no sugerir */
}

/**
 * @brief Arma el prompt: SO_TacoDiPapa@interprete:~/ruta >>>
 */
static void buildPrompt(char *prompt, size_t tamano, int ultimoFallo) {
    char cwd[INPUT_SIZE];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        strcpy(cwd, "?");

    const char *home = getenv("HOME");
    size_t lenHome = home ? strlen(home) : 0;
    int enHome = lenHome > 0 && strncmp(cwd, home, lenHome) == 0 &&
                 (cwd[lenHome] == '/' || cwd[lenHome] == '\0');

    if (!editorInteractivo()) {    /* sin colores si no es una terminal */
        snprintf(prompt, tamano, "SO_TacoDiPapa@interprete:%s%s >>> ",
                 enHome ? "~" : "", enHome ? cwd + lenHome : cwd);
        return;
    }
    /* ">>>" en rojo si el ultimo comando fallo */
    snprintf(prompt, tamano,
             COLOR_GREEN "SO_TacoDiPapa" COLOR_WHITE "@" COLOR_RED "interprete"
             COLOR_WHITE ":" COLOR_BLUE "%s%s%s >>> " COLOR_RESET,
             enHome ? "~" : "", enHome ? cwd + lenHome : cwd,
             ultimoFallo ? COLOR_RED : COLOR_WHITE);
}

/**
 * @brief Ctrl+C no cierra el interprete. Solo usa write() (printf no es
 *        seguro dentro de un manejador de senales).
 */
static void sigintHandler(int sig) {
    (void)sig;
    if (write(STDOUT_FILENO, "\n", 1) < 0) { /* nada que hacer */ }
}

int main(void) {
    char input[INPUT_SIZE];
    char expandida[INPUT_SIZE];
    char prompt[INPUT_SIZE + 128];

    /* Sin SA_RESTART: las lecturas regresan con EINTR al recibir Ctrl+C.
       Ctrl+\ tampoco debe cerrar el interprete. */
    struct sigaction sa = { .sa_handler = sigintHandler };
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    /* Sin control de trabajos: Ctrl+Z dejaria al interprete esperando para
       siempre a un hijo detenido, asi que se ignora (los hijos lo heredan) */
    signal(SIGTSTP, SIG_IGN);

    listaInicializar(&listaProcesosGlobal);
    memoriaInicializar(&memoriaGlobal);
    editorInicializar();
    atexit(editorRestaurar);
    historialInicializar(editorInteractivo());

    if (editorInteractivo()) {
        printf(COLOR_GREEN "SO_TacoDiPapa@interprete" COLOR_RESET
               "  |  Escriba " ANSI_NEGRITA "info" COLOR_RESET
               " para ver los comandos disponibles.\n");
        printf(ANSI_TENUE "Flechas arriba/abajo: historial  |  TAB: autocompletar  |  "
               "Ctrl+D: salir" COLOR_RESET "\n\n");
    }

    int ultimoFallo = 0;
    while (1) {
        buildPrompt(prompt, sizeof(prompt), ultimoFallo);

        ResultadoLectura leida = editorLeerLinea(prompt, input, sizeof(input), 1);
        if (leida == LECTURA_FIN_ENTRADA) {     /* Ctrl+D o fin de entrada */
            if (editorInteractivo()) printf("exit\n");
            break;
        }
        if (leida == LECTURA_CANCELADA)         /* Ctrl+C: nueva linea */
            continue;
        if (input[strspn(input, " \t")] == '\0')
            continue;

        /* !!, !n, !texto: repetir un comando del historial */
        reiniciarErrorComando();
        int expansion = historialExpandir(input, expandida, sizeof(expandida));
        if (expansion < 0) {
            ultimoFallo = 1;
            continue;
        }
        if (expansion > 0) {
            snprintf(input, sizeof(input), "%s", expandida);
            printf("%s%s%s\n", colorSalida(ANSI_TENUE), input, colorSalida(ANSI_RESET));
        }
        historialAgregar(input);

        /* Un solo comando interno: se ejecuta en el propio interprete
           (asi cd, mkprocess, etc. afectan a este proceso) */
        static Pipeline p;
        if (parser(input, &p) != 0) {
            ultimoFallo = 1;
            continue;
        }
        if (p.ncommands == 1 && isBuiltin(p.args[0][0])) {
            if (runBuiltin(p.args[0]) == 1)
                break;
            fflush(stdout);
            ultimoFallo = huboErrorComando();
            continue;
        }

        /* Comandos externos y tuberias: un proceso hijo por comando */
        ultimoFallo = runPipeline(input) != 0;
    }

    historialLiberar();
    return 0;
}
