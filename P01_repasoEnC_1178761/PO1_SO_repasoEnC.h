#ifndef P01_SO_repasoEnC_H
#define P01_SO_repasoEnC_H

#include "List.h"

#define CREATURES_FILE "creatures"

/**
 * @struct Creature
 * @brief  Estructura para almacenar los datos de una criatura digital.
 */
typedef struct Creature {
	int id;            /**< Clave unica (irrepetible) */
	char specie[50];   /**< Especie de la criatura    */
	int level;         /**< Nivel de la criatura      */
	int exp;           /**< Puntos de experiencia     */
	char nickname[30]; /**< Apodo de la criatura      */
} Creature;

/* Funciones auxiliares */
void cleanBuffer(void);

/**
 * @brief  Busca una criatura por ID en la lista.
 * @param  list Lista donde se busca.
 * @param  id   ID unico a buscar.
 * @return Indice del nodo o -1 si no fue encontrado.
 */
int searchIndexById(List list, int id);

/* Funciones de archivo y lista */

/**
 * @brief Apertura 1: Carga el archivo 'creatures' a la lista al iniciar.
 * @param list Puntero doble a la lista.
 */
void loadFile(List* list);

/**
 * @brief Apertura 2: Sobreescribe el archivo 'creatures' con la lista al salir.
 * @param list Lista con los datos a persistir.
 */
void saveFile(List list);

/**
 * @brief Captura una criatura por teclado y la agrega a la lista.
 * @param list Puntero doble a la lista.
 */
void captureCreature(List* list);

/**
 * @brief Muestra todas las criaturas de la lista en consola.
 * @param list Lista a visualizar.
 */
void showCreatures(List list);

/**
 * @brief Elimina una criatura de la lista por ID y libera su memoria.
 * @param list Puntero doble a la lista.
 */
void deleteCreature(List* list);

#endif /* P01_SO_repasoEnC_H */


