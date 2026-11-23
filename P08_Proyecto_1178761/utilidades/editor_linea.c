/**
 * @file  editor_linea.c
 * @brief Editor de linea en modo raw con historial y autocompletado.
 *
 * Redibujado: en cada cambio se sube el cursor al renglon donde inicia el
 * prompt, se borra hasta el final de la pantalla y se vuelve a escribir
 * prompt + linea, colocando despues el cursor en su renglon y columna.
 * Asi las lineas mas largas que la terminal (que ocupan varios renglones)
 * se editan correctamente.
 */
#define _GNU_SOURCE
#include <errno.h>
#include <poll.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "editor_linea.h"
#include "autocompletado.h"
#include "historial.h"
#include "terminal.h"

#define TAMANO_MAXIMO_LINEA 2048

/* Codigos de teclas de control */
#define TECLA_CTRL_A      1
#define TECLA_CTRL_B      2
#define TECLA_CTRL_C      3
#define TECLA_CTRL_D      4
#define TECLA_CTRL_E      5
#define TECLA_CTRL_F      6
#define TECLA_CTRL_H      8
#define TECLA_TAB         9
#define TECLA_CTRL_J      10
#define TECLA_CTRL_K      11
#define TECLA_CTRL_L      12
#define TECLA_ENTER       13
#define TECLA_CTRL_N      14
#define TECLA_CTRL_P      16
#define TECLA_CTRL_U      21
#define TECLA_CTRL_W      23
#define TECLA_ESCAPE      27
#define TECLA_RETROCESO   127

/** Candidatos maximos que se listan con doble TAB. */
#define MAXIMO_CANDIDATOS_MOSTRADOS 200

/** Tamano del buffer de salida de un redibujado. */
#define TAMANO_BUFFER_SALIDA (TAMANO_MAXIMO_LINEA * 4 + 1024)

/** Configuracion original de la terminal. */
static struct termios configuracionOriginalTerminal;
static int terminalConfigurada = 0;
static int modoInteractivo = 0;

/** Estado de la linea en edicion. */
typedef struct {
    char *texto;                 /**< contenido de la linea */
    size_t capacidad;            /**< bytes maximos (incluye '\0') */
    size_t longitud;             /**< bytes usados */
    size_t cursor;               /**< posicion del cursor en bytes */
    const char *prompt;
    int columnasPrompt;          /**< ancho visible del prompt */
    int filaCursorAnterior;      /**< renglon del cursor tras el ultimo dibujo */
    int usarHistorial;
    int posicionHistorial;       /**< == cantidad de entradas: linea propia */
    char lineaPropia[TAMANO_MAXIMO_LINEA]; /**< lo escrito antes de navegar */
} EstadoEditor;

/* --------------------------- Configuracion ----------------------------- */

void editorInicializar(void) {
    const char *tipoTerminal = getenv("TERM");
    int terminalSoportada = tipoTerminal == NULL ||
                            (strcasecmp(tipoTerminal, "dumb") != 0 &&
                             strcasecmp(tipoTerminal, "cons25") != 0);

    modoInteractivo = isatty(STDIN_FILENO) && isatty(STDOUT_FILENO) && terminalSoportada &&
                      tcgetattr(STDIN_FILENO, &configuracionOriginalTerminal) == 0;
    terminalConfigurada = modoInteractivo;
}

int editorInteractivo(void) {
    return modoInteractivo;
}

void editorRestaurar(void) {
    if (terminalConfigurada)
        tcsetattr(STDIN_FILENO, TCSADRAIN, &configuracionOriginalTerminal);
}

/**
 * @brief Activa el modo raw: sin eco, sin buffer de linea y sin senales
 *        de teclado (Ctrl+C llega como un caracter).
 */
static int activarModoRaw(void) {
    struct termios configuracionRaw = configuracionOriginalTerminal;

    configuracionRaw.c_iflag &= ~(tcflag_t)(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    configuracionRaw.c_cflag |= CS8;
    configuracionRaw.c_lflag &= ~(tcflag_t)(ECHO | ICANON | IEXTEN | ISIG);
    configuracionRaw.c_cc[VMIN] = 1;
    configuracionRaw.c_cc[VTIME] = 0;
    /* c_oflag se conserva: "\n" se sigue traduciendo a "\r\n" */
    return tcsetattr(STDIN_FILENO, TCSADRAIN, &configuracionRaw);
}

/* ---------------------------- Utilidades -------------------------------- */


/** @brief 1 si el byte es continuacion de un caracter UTF-8 multibyte. */
static int esByteContinuacionUtf8(unsigned char byte) {
    return (byte & 0xC0) == 0x80;
}

/**
 * @brief Columnas que ocupa un texto: ignora secuencias ANSI y cuenta un
 *        caracter UTF-8 como una columna.
 */
static int contarColumnasVisibles(const char *texto, size_t longitud) {
    int columnas = 0;
    for (size_t indice = 0; indice < longitud; indice++) {
        unsigned char byte = (unsigned char)texto[indice];
        if (byte == '\033' && indice + 1 < longitud && texto[indice + 1] == '[') {
            indice += 2;
            while (indice < longitud && !(texto[indice] >= '@' && texto[indice] <= '~'))
                indice++;
            continue;
        }
        if (!esByteContinuacionUtf8(byte))
            columnas++;
    }
    return columnas;
}

/** @brief Escribe todo el buffer en stdout (reintentando si se interrumpe). */
static void escribirTerminal(const char *datos, size_t longitud) {
    while (longitud > 0) {
        ssize_t escritos = write(STDOUT_FILENO, datos, longitud);
        if (escritos < 0) {
            if (errno == EINTR)
                continue;
            return;
        }
        datos += escritos;
        longitud -= (size_t)escritos;
    }
}

/** @brief Escribe una cadena terminada en '\0'. */
static void escribirCadena(const char *cadena) {
    escribirTerminal(cadena, strlen(cadena));
}

/** @brief Emite el sonido de alerta de la terminal. */
static void emitirAlerta(void) {
    escribirCadena("\a");
}

/**
 * @brief Lee un byte de la entrada.
 * @return 1 si se leyo, 0 en fin de entrada, -1 en error.
 */
static int leerByte(unsigned char *byte) {
    for (;;) {
        ssize_t leidos = read(STDIN_FILENO, byte, 1);
        if (leidos >= 0)
            return (int)leidos;
        if (errno != EINTR)
            return -1;
    }
}

/**
 * @brief Consulta a la terminal la columna actual del cursor (1 = inicio).
 * @return La columna, o -1 si la terminal no respondio a tiempo.
 */
static int consultarColumnaCursor(void) {
    char respuesta[32];
    size_t longitudRespuesta = 0;
    int fila, columna;

    /* Si el usuario ya escribio algo (mientras corria el comando anterior)
       no se consulta: la respuesta se mezclaria con esas teclas y se perderian */
    struct pollfd pendiente = {.fd = STDIN_FILENO, .events = POLLIN};
    if (poll(&pendiente, 1, 0) > 0)
        return -1;

    escribirCadena("\033[6n");
    while (longitudRespuesta < sizeof(respuesta) - 1) {
        struct pollfd descriptorEntrada = {.fd = STDIN_FILENO, .events = POLLIN};
        if (poll(&descriptorEntrada, 1, 100) <= 0)
            break;
        if (read(STDIN_FILENO, &respuesta[longitudRespuesta], 1) != 1)
            break;
        if (respuesta[longitudRespuesta++] == 'R')
            break;
    }
    respuesta[longitudRespuesta] = '\0';

    if (respuesta[0] != '\033' || sscanf(respuesta + 1, "[%d;%dR", &fila, &columna) != 2)
        return -1;
    return columna;
}

/* ---------------------------- Redibujado -------------------------------- */

/** Buffer donde se arma un redibujado completo antes de escribirlo. */
typedef struct {
    char datos[TAMANO_BUFFER_SALIDA];
    size_t usado;
} BufferSalida;

/**
 * @brief Agrega texto con formato al buffer (se descarta lo que no quepa).
 */
static void agregarSalida(BufferSalida *buffer, const char *formato, ...)
    __attribute__((format(printf, 2, 3)));

static void agregarSalida(BufferSalida *buffer, const char *formato, ...) {
    size_t disponible = sizeof(buffer->datos) - buffer->usado;
    va_list argumentosVariables;

    if (disponible <= 1)
        return;
    va_start(argumentosVariables, formato);
    int escritos = vsnprintf(buffer->datos + buffer->usado, disponible, formato,
                             argumentosVariables);
    va_end(argumentosVariables);

    if (escritos > 0)
        buffer->usado += (size_t)escritos < disponible ? (size_t)escritos : disponible - 1;
}

/**
 * @brief Redibuja prompt + linea y coloca el cursor en su lugar.
 */
static void redibujarLinea(EstadoEditor *estado) {
    static BufferSalida salida;
    int anchoTerminal = obtenerAnchoTerminal();

    salida.usado = 0;

    /* 1. Subir al renglon del prompt y borrar todo lo anterior */
    if (estado->filaCursorAnterior > 0)
        agregarSalida(&salida, "\033[%dA", estado->filaCursorAnterior);
    agregarSalida(&salida, "\r\033[J");

    /* 2. Escribir prompt y linea */
    agregarSalida(&salida, "%s%.*s", estado->prompt, (int)estado->longitud, estado->texto);

    int columnasTotales = estado->columnasPrompt +
                          contarColumnasVisibles(estado->texto, estado->longitud);
    int columnasHastaCursor = estado->columnasPrompt +
                              contarColumnasVisibles(estado->texto, estado->cursor);

    /* Si el texto llena exactamente el renglon, la terminal deja el cursor
       "pendiente" en la ultima columna: se fuerza el salto de renglon */
    if (columnasTotales > 0 && columnasTotales % anchoTerminal == 0)
        agregarSalida(&salida, "\n");

    /* 3. Llevar el cursor desde el final del texto hasta su posicion */
    int filaFinal = columnasTotales / anchoTerminal;
    int filaCursor = columnasHastaCursor / anchoTerminal;
    int columnaCursor = columnasHastaCursor % anchoTerminal;

    if (filaFinal > filaCursor)
        agregarSalida(&salida, "\033[%dA", filaFinal - filaCursor);
    agregarSalida(&salida, "\r");
    if (columnaCursor > 0)
        agregarSalida(&salida, "\033[%dC", columnaCursor);

    estado->filaCursorAnterior = filaCursor;
    escribirTerminal(salida.datos, salida.usado);
}

/**
 * @brief Lleva el cursor al final de la linea (para escribir debajo de ella).
 */
static void moverCursorAlFinal(EstadoEditor *estado) {
    estado->cursor = estado->longitud;
    redibujarLinea(estado);
}

/* ----------------------------- Edicion ---------------------------------- */

/** @brief Inserta un byte en la posicion del cursor. */
static void insertarByte(EstadoEditor *estado, char byte) {
    if (estado->longitud + 1 >= estado->capacidad) {
        emitirAlerta();
        return;
    }
    memmove(estado->texto + estado->cursor + 1, estado->texto + estado->cursor,
            estado->longitud - estado->cursor);
    estado->texto[estado->cursor++] = byte;
    estado->longitud++;
    estado->texto[estado->longitud] = '\0';
}

/** @brief Borra los bytes [desde, hasta) y deja el cursor en desde. */
static void borrarRango(EstadoEditor *estado, size_t desde, size_t hasta) {
    if (hasta <= desde)
        return;
    memmove(estado->texto + desde, estado->texto + hasta, estado->longitud - hasta);
    estado->longitud -= hasta - desde;
    estado->texto[estado->longitud] = '\0';
    estado->cursor = desde;
}

/** @brief Posicion del caracter UTF-8 anterior al cursor. */
static size_t posicionCaracterAnterior(const EstadoEditor *estado, size_t posicion) {
    if (posicion == 0)
        return 0;
    do {
        posicion--;
    } while (posicion > 0 && esByteContinuacionUtf8((unsigned char)estado->texto[posicion]));
    return posicion;
}

/** @brief Posicion del caracter UTF-8 siguiente al cursor. */
static size_t posicionCaracterSiguiente(const EstadoEditor *estado, size_t posicion) {
    if (posicion >= estado->longitud)
        return estado->longitud;
    do {
        posicion++;
    } while (posicion < estado->longitud &&
             esByteContinuacionUtf8((unsigned char)estado->texto[posicion]));
    return posicion;
}

/** @brief Borra la palabra anterior al cursor (Ctrl+W). */
static void borrarPalabraAnterior(EstadoEditor *estado) {
    size_t inicio = estado->cursor;
    while (inicio > 0 && estado->texto[inicio - 1] == ' ')
        inicio--;
    while (inicio > 0 && estado->texto[inicio - 1] != ' ')
        inicio--;
    borrarRango(estado, inicio, estado->cursor);
}

/** @brief Reemplaza todo el contenido de la linea. */
static void reemplazarLinea(EstadoEditor *estado, const char *nuevoTexto) {
    snprintf(estado->texto, estado->capacidad, "%s", nuevoTexto);
    estado->longitud = strlen(estado->texto);
    estado->cursor = estado->longitud;
}

/* ----------------------------- Historial -------------------------------- */

/**
 * @brief Mueve la posicion del historial (-1 = anterior, +1 = siguiente).
 */
static void navegarHistorial(EstadoEditor *estado, int direccion) {
    int cantidadEntradas = historialCantidad();

    if (!estado->usarHistorial || cantidadEntradas == 0) {
        emitirAlerta();
        return;
    }
    if (direccion < 0 && estado->posicionHistorial == 0) {
        emitirAlerta();
        return;
    }
    if (direccion > 0 && estado->posicionHistorial >= cantidadEntradas) {
        emitirAlerta();
        return;
    }

    /* Al salir de la linea propia se guarda lo que el usuario escribio */
    if (estado->posicionHistorial == cantidadEntradas)
        snprintf(estado->lineaPropia, sizeof(estado->lineaPropia), "%s", estado->texto);

    estado->posicionHistorial += direccion;
    if (estado->posicionHistorial == cantidadEntradas)
        reemplazarLinea(estado, estado->lineaPropia);
    else
        reemplazarLinea(estado, historialObtener(estado->posicionHistorial));
}

/* --------------------------- Autocompletado ----------------------------- */

/**
 * @brief Imprime los candidatos en columnas debajo de la linea actual.
 */
static void mostrarCandidatos(EstadoEditor *estado, const ListaCandidatos *lista) {
    size_t anchoColumna = 0;
    for (size_t indice = 0; indice < lista->cantidad; indice++) {
        size_t longitud = strlen(lista->candidatos[indice]);
        if (longitud > anchoColumna)
            anchoColumna = longitud;
    }
    anchoColumna += 2;

    size_t columnasPorRenglon = (size_t)obtenerAnchoTerminal() / anchoColumna;
    if (columnasPorRenglon == 0)
        columnasPorRenglon = 1;

    moverCursorAlFinal(estado);
    escribirCadena("\n");

    size_t mostrados = lista->cantidad < MAXIMO_CANDIDATOS_MOSTRADOS
                           ? lista->cantidad : MAXIMO_CANDIDATOS_MOSTRADOS;
    char celda[4096];
    for (size_t indice = 0; indice < mostrados; indice++) {
        int esUltimaColumna = (indice + 1) % columnasPorRenglon == 0 || indice + 1 == mostrados;
        snprintf(celda, sizeof(celda), "%-*s%s", (int)anchoColumna, lista->candidatos[indice],
                 esUltimaColumna ? "\n" : "");
        escribirCadena(celda);
    }
    if (mostrados < lista->cantidad) {
        snprintf(celda, sizeof(celda), "... y %zu mas\n", lista->cantidad - mostrados);
        escribirCadena(celda);
    }
    estado->filaCursorAnterior = 0;   /* el prompt se dibuja en un renglon nuevo */
}

/**
 * @brief Maneja la tecla TAB.
 * @param tabAnterior 1 si la tecla previa tambien fue TAB.
 */
static void autocompletarPalabra(EstadoEditor *estado, int tabAnterior) {
    ListaCandidatos lista;
    size_t posicionCursorOriginal = estado->cursor;
    size_t inicioPalabra = buscarCandidatosCompletado(estado->texto, estado->cursor, &lista);
    size_t longitudPalabra = estado->cursor - inicioPalabra;

    if (lista.cantidad == 0) {
        emitirAlerta();
        liberarCandidatos(&lista);
        return;
    }

    size_t longitudComun = calcularPrefijoComun(&lista);
    if (lista.cantidad == 1 || longitudComun > longitudPalabra) {
        const char *candidato = lista.candidatos[0];
        borrarRango(estado, inicioPalabra, posicionCursorOriginal);
        for (size_t indice = 0; indice < longitudComun; indice++)
            insertarByte(estado, candidato[indice]);
        /* Candidato unico que no es directorio: se agrega un espacio */
        if (lista.cantidad == 1 && candidato[longitudComun - 1] != '/')
            insertarByte(estado, ' ');
    } else if (tabAnterior) {
        mostrarCandidatos(estado, &lista);
    } else {
        emitirAlerta();
    }
    liberarCandidatos(&lista);
}

/* --------------------------- Teclas especiales -------------------------- */

/** Acciones que puede producir una secuencia de escape. */
typedef enum {
    ACCION_NINGUNA, ACCION_ARRIBA, ACCION_ABAJO, ACCION_DERECHA, ACCION_IZQUIERDA,
    ACCION_INICIO, ACCION_FIN, ACCION_SUPRIMIR
} AccionEscape;

/**
 * @brief Interpreta la secuencia que sigue a ESC (flechas, Inicio, Fin, Supr).
 */
static AccionEscape leerSecuenciaEscape(void) {
    unsigned char secuencia[3];

    if (leerByte(&secuencia[0]) != 1 || leerByte(&secuencia[1]) != 1)
        return ACCION_NINGUNA;

    if (secuencia[0] == '[' && secuencia[1] >= '0' && secuencia[1] <= '9') {
        if (leerByte(&secuencia[2]) != 1)
            return ACCION_NINGUNA;
        /* Descartar modificadores como "1;5C" (Ctrl+flecha) */
        while (secuencia[2] != '~' && !(secuencia[2] >= 'A' && secuencia[2] <= 'Z'))
            if (leerByte(&secuencia[2]) != 1)
                return ACCION_NINGUNA;
        if (secuencia[2] != '~') {
            secuencia[1] = secuencia[2];   /* "[1;5C" se trata como "[C" */
        } else {
            switch (secuencia[1]) {
            case '1': case '7': return ACCION_INICIO;
            case '4': case '8': return ACCION_FIN;
            case '3':           return ACCION_SUPRIMIR;
            default:            return ACCION_NINGUNA;
            }
        }
    }

    if (secuencia[0] == '[' || secuencia[0] == 'O') {
        switch (secuencia[1]) {
        case 'A': return ACCION_ARRIBA;
        case 'B': return ACCION_ABAJO;
        case 'C': return ACCION_DERECHA;
        case 'D': return ACCION_IZQUIERDA;
        case 'H': return ACCION_INICIO;
        case 'F': return ACCION_FIN;
        default:  return ACCION_NINGUNA;
        }
    }
    return ACCION_NINGUNA;
}

/**
 * @brief Aplica la accion de una secuencia de escape.
 */
static void aplicarAccionEscape(EstadoEditor *estado, AccionEscape accion) {
    switch (accion) {
    case ACCION_ARRIBA:    navegarHistorial(estado, -1); break;
    case ACCION_ABAJO:     navegarHistorial(estado, +1); break;
    case ACCION_DERECHA:   estado->cursor = posicionCaracterSiguiente(estado, estado->cursor); break;
    case ACCION_IZQUIERDA: estado->cursor = posicionCaracterAnterior(estado, estado->cursor); break;
    case ACCION_INICIO:    estado->cursor = 0; break;
    case ACCION_FIN:       estado->cursor = estado->longitud; break;
    case ACCION_SUPRIMIR:
        borrarRango(estado, estado->cursor, posicionCaracterSiguiente(estado, estado->cursor));
        break;
    case ACCION_NINGUNA:
    default:
        break;
    }
}

/* ------------------------------ Lectura --------------------------------- */

/**
 * @brief Bucle de edicion en modo raw.
 */
static ResultadoLectura editarLineaInteractiva(EstadoEditor *estado) {
    int tabAnterior = 0;

    /* Si la salida anterior no termino en salto de linea, el prompt se
       dibujaria encima de ella: se marca el corte y se baja de renglon */
    if (consultarColumnaCursor() > 1) {
        char marcaSinSalto[64];
        snprintf(marcaSinSalto, sizeof(marcaSinSalto), "%s⏎%s\n", ANSI_TENUE, ANSI_RESET);
        escribirCadena(marcaSinSalto);
    }
    redibujarLinea(estado);

    for (;;) {
        unsigned char tecla;
        int resultadoLectura = leerByte(&tecla);
        if (resultadoLectura <= 0) {
            moverCursorAlFinal(estado);
            return estado->longitud > 0 ? LECTURA_EXITOSA : LECTURA_FIN_ENTRADA;
        }

        int esTab = tecla == TECLA_TAB;
        switch (tecla) {
        case TECLA_ENTER:
        case TECLA_CTRL_J:
            moverCursorAlFinal(estado);
            escribirCadena("\n");
            return LECTURA_EXITOSA;

        case TECLA_CTRL_C:
            moverCursorAlFinal(estado);
            escribirCadena("^C\n");
            estado->longitud = 0;
            estado->texto[0] = '\0';
            return LECTURA_CANCELADA;

        case TECLA_CTRL_D:
            if (estado->longitud == 0) {
                moverCursorAlFinal(estado);
                return LECTURA_FIN_ENTRADA;
            }
            borrarRango(estado, estado->cursor, posicionCaracterSiguiente(estado, estado->cursor));
            break;

        case TECLA_RETROCESO:
        case TECLA_CTRL_H:
            borrarRango(estado, posicionCaracterAnterior(estado, estado->cursor), estado->cursor);
            break;

        case TECLA_TAB:
            if (estado->usarHistorial)
                autocompletarPalabra(estado, tabAnterior);
            else
                insertarByte(estado, ' ');
            break;

        case TECLA_ESCAPE:
            aplicarAccionEscape(estado, leerSecuenciaEscape());
            break;

        case TECLA_CTRL_A: estado->cursor = 0; break;
        case TECLA_CTRL_E: estado->cursor = estado->longitud; break;
        case TECLA_CTRL_B: aplicarAccionEscape(estado, ACCION_IZQUIERDA); break;
        case TECLA_CTRL_F: aplicarAccionEscape(estado, ACCION_DERECHA); break;
        case TECLA_CTRL_P: navegarHistorial(estado, -1); break;
        case TECLA_CTRL_N: navegarHistorial(estado, +1); break;
        case TECLA_CTRL_U: borrarRango(estado, 0, estado->cursor); break;
        case TECLA_CTRL_K: borrarRango(estado, estado->cursor, estado->longitud); break;
        case TECLA_CTRL_W: borrarPalabraAnterior(estado); break;

        case TECLA_CTRL_L:
            escribirCadena("\033[H\033[2J");
            estado->filaCursorAnterior = 0;
            break;

        default:
            /* Caracteres imprimibles (incluye bytes UTF-8); el resto se ignora */
            if (tecla >= 32)
                insertarByte(estado, (char)tecla);
            break;
        }
        tabAnterior = esTab;
        redibujarLinea(estado);
    }
}

/**
 * @brief Lectura simple con fgets cuando no hay terminal interactiva.
 */
static ResultadoLectura leerLineaSimple(const char *prompt, char *destino, size_t tamanoDestino) {
    if (isatty(STDIN_FILENO)) {
        fputs(prompt, stdout);
        fflush(stdout);
    }

    errno = 0;
    if (fgets(destino, (int)tamanoDestino, stdin) == NULL) {
        if (errno == EINTR) {       /* Ctrl+C con terminal sin editor */
            clearerr(stdin);
            putchar('\n');
            return LECTURA_CANCELADA;
        }
        return LECTURA_FIN_ENTRADA;
    }

    size_t longitud = strcspn(destino, "\n");
    if (destino[longitud] != '\n' && !feof(stdin)) {
        /* Linea mas larga que el buffer: se descarta el resto */
        int caracter;
        while ((caracter = getchar()) != '\n' && caracter != EOF)
            ;
        mostrarError(NULL, "la linea excede el maximo de %zu caracteres", tamanoDestino - 1);
        destino[0] = '\0';
        return LECTURA_CANCELADA;
    }
    destino[longitud] = '\0';
    if (longitud > 0 && destino[longitud - 1] == '\r')
        destino[longitud - 1] = '\0';
    return LECTURA_EXITOSA;
}

ResultadoLectura editorLeerLinea(const char *prompt, char *destino, size_t tamanoDestino,
                           int usarHistorial) {
    if (tamanoDestino == 0)
        return LECTURA_FIN_ENTRADA;
    destino[0] = '\0';

    if (!modoInteractivo || activarModoRaw() != 0)
        return leerLineaSimple(prompt, destino, tamanoDestino);

    EstadoEditor *estado = calloc(1, sizeof(EstadoEditor));
    if (estado == NULL) {
        editorRestaurar();
        return leerLineaSimple(prompt, destino, tamanoDestino);
    }
    estado->texto = destino;
    estado->capacidad = tamanoDestino;
    estado->prompt = prompt;
    estado->columnasPrompt = contarColumnasVisibles(prompt, strlen(prompt));
    estado->usarHistorial = usarHistorial;
    estado->posicionHistorial = historialCantidad();

    fflush(stdout);
    ResultadoLectura resultado = editarLineaInteractiva(estado);
    editorRestaurar();
    free(estado);
    return resultado;
}
