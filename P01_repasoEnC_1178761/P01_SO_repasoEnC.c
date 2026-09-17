#define CREATURES_FILE "creatures"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Llamadas al sistema de bajo nivel (POSIX / Linux) */
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "List.h"
#include "PO1_SO_repasoEnC.h"

/**
 * @brief  Punto de entrada principal del programa.
 * @return int Codigo de salida del proceso.
 */
int main(void) {
	/* 1. Cargar datos del archivo a la lista al iniciar */
	List list = createList();
	loadFile(&list);

	int option;
	do {
		printf("\n======= MENU =======\n");
		printf("1. Capturar criatura\n");
		printf("2. Mostrar criaturas\n");
		printf("3. Eliminar criatura\n");
		printf("4. Salir\n");
		printf("======================\n");
		printf("Seleccione una opcion: ");

		if (scanf("%d", &option) != 1) {
			printf("Entrada invalida. Se esperaba un numero entero.\n");
			cleanBuffer();
			continue;
		}

		switch (option) {
		case 1: captureCreature(&list); break;
		case 2: showCreatures(list); break;
		case 3: deleteCreature(&list); break;
		case 4: printf("Saliendo...\n"); break;
		default:
			printf("Opcion no valida.\n");
			break;
		}
	} while (option != 4);

	/* 2. Guardar lista en archivo al salir y liberar memoria */
	saveFile(list);
	destroyList(&list, free);

	return 0;
}

/**
 * @brief Limpia los caracteres residuales del buffer del teclado (stdin).
 */
void cleanBuffer(void) {
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * @brief  Busca una criatura por su ID en la lista.
 * @param  list Lista donde se busca.
 * @param  id   Identificador unico a localizar.
 * @return Indice base 0 si existe, o -1 si no fue encontrada.
 */
int searchIndexById(List list, int id) {
	int total = sizeList(list);
	for (int i = 0; i < total; i++) {
		Creature* c = (Creature*)getList(list, i);
		if (c != NULL && c->id == id) {
			return i;
		}
	}
	return -1;
}

/**
 * @brief Apertura 1: Carga secuencialmente el archivo a la lista al iniciar.
 * @param list Puntero doble a la lista para insertar nodos.
 */
void loadFile(List* list) {
	int file = open(CREATURES_FILE, O_RDONLY);
	if (file == -1) {
		printf("No se encontro archivo previo. Se iniciara una lista vacia.\n");
		return;
	}

	Creature temp;
	while (read(file, &temp, sizeof(Creature)) == sizeof(Creature)) {
		Creature* c = (Creature*)malloc(sizeof(Creature));
		if (c != NULL) {
			*c = temp;
			listInsertEnd(list, c);
		}
	}

	close(file);
	printf("Archivo cargado correctamente (%d criatura(s) en memoria).\n", sizeList(*list));
}

/**
 * @brief Apertura 2: Guarda la lista completa en el archivo al salir.
 * @param list Lista con los datos en memoria a persistir.
 */
void saveFile(List list) {
	int file = open(CREATURES_FILE, 
	                O_WRONLY | O_CREAT | O_TRUNC, 
	                S_IRUSR | S_IWUSR);
	if (file == -1) {
		printf("Error al abrir el archivo para guardar.\n");
		return;
	}

	int total = sizeList(list);
	for (int i = 0; i < total; i++) {
		Creature* c = (Creature*)getList(list, i);
		if (c != NULL) {
			write(file, c, sizeof(Creature));
		}
	}

	close(file);
	printf("Archivo guardado correctamente (%d criatura(s) guardadas).\n", total);
}

/**
 * @brief Captura una criatura por teclado y la agrega a la lista en RAM.
 * @param list Puntero doble a la lista donde se insertara el nuevo nodo.
 */
void captureCreature(List* list) {
	int id;
	printf("\n===== Captura de criatura =====\n");
	printf("Ingrese el ID de la criatura: ");
	if (scanf("%d", &id) != 1) {
		printf("Entrada invalida. Se esperaba un numero entero.\n");
		cleanBuffer();
		return;
	}

	if (searchIndexById(*list, id) != -1) {
		printf("Error: Ya existe una criatura registrada con el ID %d.\n", id);
		return;
	}

	Creature* c = (Creature*)malloc(sizeof(Creature));
	if (c == NULL) {
		printf("Error al asignar memoria.\n");
		return;
	}

	c->id = id;
	printf("Especie: ");
	scanf("%s", c->specie);
	printf("Nivel: ");
	scanf("%d", &c->level);
	printf("Experiencia: ");
	scanf("%d", &c->exp);
	printf("Apodo: ");
	scanf("%s", c->nickname);

	listInsertEnd(list, c);
	printf("\nCriatura capturada exitosamente.\n");
}

/**
 * @brief Muestra en consola todas las criaturas registradas en formato tabular.
 * @param list Lista de criaturas a visualizar.
 */
void showCreatures(List list) {
	int total = sizeList(list);
	printf("\n========================================================================\n");
	printf("                       REGISTRO DE CRIATURAS DIGITALES                  \n");
	printf("========================================================================\n");

	if (total == 0) {
		printf(" Lista Vacia actualmente.\n");
		return;
	}

	printf("%-8s | %-20s | %-8s | %-12s | %-18s\n", "CLAVE", "ESPECIE", "NIVEL", "EXPERIENCIA", "APODO");
	printf("---------+----------------------+----------+--------------+-------------\n");
	for (int i = 0; i < total; i++) {
		Creature* c = (Creature*)getList(list, i);
		if (c != NULL) {
			printf("%-8d | %-20s | %-8d | %-12d | %-18s\n", 
			       c->id, c->specie, c->level, c->exp, c->nickname);
		}
	}
	printf("========================================================================\n");
	printf(" Total: %d criatura(s)\n", total);
}

/**
 * @brief Elimina una criatura de la lista por ID y libera su memoria Heap.
 * @param list Puntero doble a la lista para actualizar los enlaces.
 */
void deleteCreature(List* list) {
	if (sizeList(*list) == 0) {
		printf("\n No hay criaturas para eliminar.\n");
		return;
	}

	int id;
	printf("\n===== Eliminacion de criatura =====\n");
	printf("Ingrese el ID de la criatura a eliminar: ");
	if (scanf("%d", &id) != 1) {
		printf("Entrada invalida. Se esperaba un numero entero.\n");
		cleanBuffer();
		return;
	}

	int index = searchIndexById(*list, id);
	if (index == -1) {
		printf("No se encontro una criatura con el ID %d.\n", id);
		return;
	}

	Creature* kill = (Creature*)listRemove(list, index);
	if (kill == NULL) {
		printf("Error al eliminar la criatura.\n");
		return;
	}

	free(kill);
	printf("Criatura eliminada exitosamente.\n");
}