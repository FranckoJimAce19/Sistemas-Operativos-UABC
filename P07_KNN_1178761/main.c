/**
 * @file  main.c
 * @brief Bucle principal del interprete de comandos — Practicas 6-7 SO.
 */
#define _GNU_SOURCE
#include <errno.h>
#include "main.h"

/** Lista de procesos global para CPU Scheduling (Practica 6) */
ListaProcesos listaProcesosGlobal;

/* ---------- Adaptadores: todos los builtins reciben args ---------- */
static void bInfo(char **a)      { (void)a; info(); }
static void bCd(char **a)        { cd(a[1]); }
static void bSdir(char **a)      { (void)a; sdir(); }
static void bClear(char **a)     { (void)a; clearScreen(); }
static void bJoinWrite(char **a) { joinWrite(a[1]); }
static void bJoinRead(char **a)  { joinRead(a[1]); }
static void bCopy(char **a)      { copy(a[1], a[2]); }
static void bDel(char **a)       { del(a[1]); }
static void bMkprocess(char **a) { cmdMkprocess(&listaProcesosGlobal, a); }
static void bLstprocss(char **a) { (void)a; cmdLstprocss(&listaProcesosGlobal); }
static void bFcfs(char **a)      { (void)a; cmdFcfs(&listaProcesosGlobal); }
static void bSjf(char **a)       { (void)a; cmdSjf(&listaProcesosGlobal); }
static void bRr(char **a)        { (void)a; cmdRr(&listaProcesosGlobal); }

/** Tabla de comandos internos. 'exit' se maneja aparte en runBuiltin. */
static const struct {
    const char *nombre;
    void (*fn)(char **args);
} BUILTINS[] = {
    {"info", bInfo},   {"cd", bCd},     {"sdir", bSdir},   {"clear", bClear},
    {"join>", bJoinWrite}, {"join", bJoinRead}, {"copy", bCopy}, {"del", bDel},
    /* Practica 6: planificacion de CPU */
    {"mkprocess", bMkprocess}, {"lstprocss", bLstprocss},
    {"fcfs", bFcfs}, {"sjf", bSjf}, {"rr", bRr},
    /* Practica 7: K-NN con hilos */
    {"knn", cmdKnn},
};
#define NUM_BUILTINS (sizeof(BUILTINS) / sizeof(BUILTINS[0]))

int isBuiltin(const char *cmd) {
    if (strcmp(cmd, "exit") == 0) return 1;
    for (size_t i = 0; i < NUM_BUILTINS; i++)
        if (strcmp(cmd, BUILTINS[i].nombre) == 0) return 1;
    return 0;
}

int runBuiltin(char **args) {
    if (strcmp(args[0], "exit") == 0) return 1;
    for (size_t i = 0; i < NUM_BUILTINS; i++) {
        if (strcmp(args[0], BUILTINS[i].nombre) == 0) {
            BUILTINS[i].fn(args);
            break;
        }
    }
    return 0;
}

/**
 * @brief Muestra el prompt: SO_TacoDiPapa@interprete:~/ruta >>>
 */
static void showPrompt(void) {
    char cwd[INPUT_SIZE];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        strcpy(cwd, "?");

    const char *home = getenv("HOME");
    size_t lenHome = home ? strlen(home) : 0;
    int enHome = lenHome > 0 && strncmp(cwd, home, lenHome) == 0 &&
                 (cwd[lenHome] == '/' || cwd[lenHome] == '\0');

    printf(COLOR_GREEN "SO_TacoDiPapa" COLOR_WHITE "@" COLOR_RED "interprete"
           COLOR_WHITE ":" COLOR_BLUE "%s%s" COLOR_WHITE " >>> " COLOR_RESET,
           enHome ? "~" : "", enHome ? cwd + lenHome : cwd);
    fflush(stdout);
}

/**
 * @brief Ctrl+C no cierra el interprete. Solo usa write() (printf no es
 *        seguro dentro de un manejador de senales); el prompt lo vuelve a
 *        dibujar el bucle principal cuando fgets se interrumpe.
 */
static void sigintHandler(int sig) {
    (void)sig;
    if (write(STDOUT_FILENO, "\n", 1) < 0) { /* nada que hacer */ }
}

int main(void) {
    char input[INPUT_SIZE];

    /* Sin SA_RESTART: fgets regresa con EINTR al recibir Ctrl+C */
    struct sigaction sa = { .sa_handler = sigintHandler };
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

    listaInicializar(&listaProcesosGlobal);

    printf(COLOR_GREEN "SO_TacoDiPapa@interprete" COLOR_RESET
           ": Comando 'info' para mas informacion.\n");

    while (1) {
        showPrompt();

        if (fgets(input, sizeof(input), stdin) == NULL) {
            if (errno == EINTR) {           /* Ctrl+C: nueva linea */
                errno = 0;
                clearerr(stdin);
                continue;
            }
            printf("\n");                   /* Ctrl+D o fin de entrada */
            break;
        }

        input[strcspn(input, "\n")] = '\0';
        if (input[0] == '\0')
            continue;

        /* Sin tuberia y builtin: se ejecuta en el propio interprete
           (asi cd, mkprocess, etc. afectan a este proceso) */
        if (strchr(input, '|') == NULL) {
            char copia[INPUT_SIZE];
            Pipeline p;
            snprintf(copia, sizeof(copia), "%s", input);   /* parser modifica */
            if (parser(copia, &p) != 0)
                continue;
            if (isBuiltin(p.args[0][0])) {
                if (runBuiltin(p.args[0]) == 1)
                    break;
                continue;
            }
        }

        /* Comandos externos y tuberias: un proceso hijo por comando */
        runPipeline(input);
    }

    return 0;
}
