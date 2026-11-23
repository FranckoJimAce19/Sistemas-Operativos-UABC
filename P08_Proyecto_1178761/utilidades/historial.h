/**
 * @file  historial.h
 * @brief Historial de comandos: se recorre con las flechas arriba/abajo,
 *        se consulta con 'history' y se guarda en ~/.interprete_historial.
 *
 *  Repetir comandos:  !!  ultimo  |  !n  numero n  |  !-n  n hacia atras  |
 *                     !texto  ultimo que empieza con texto
 */
#ifndef HISTORIAL_H
#define HISTORIAL_H

#define MAX_HISTORIAL     500                       /* comandos recordados */
#define ARCHIVO_HISTORIAL ".interprete_historial"   /* dentro de $HOME */

/** @brief Carga el historial; si usarArchivo es 1 lo lee y guarda en $HOME. */
void historialInicializar(int usarArchivo);

/** @brief Agrega un comando (ignora lineas vacias y repetidas seguidas). */
void historialAgregar(const char *linea);

/** @brief Cantidad de comandos guardados. */
int historialCantidad(void);

/** @brief Comando en la posicion i (0 = el mas antiguo), o NULL. */
const char *historialObtener(int i);

/**
 * @brief Expande !!, !n o !texto al inicio de la linea.
 * @return 0 sin cambios, 1 si se expandio en destino, -1 si no existe.
 */
int historialExpandir(const char *linea, char *destino, int tamano);

/** @brief Comando history: 'history', 'history n' o 'history -c'. */
void historialMostrar(char **args);

void historialLiberar(void);

#endif /* HISTORIAL_H */
