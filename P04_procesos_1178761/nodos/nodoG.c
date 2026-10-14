#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "nodos.h"

/*
 * Nodo G: muestra su mensaje y crea el archivo estructura_procesos.
 */
void nodoG(void)
{
    FILE *archivo;

    printf("Soy Nodo G, mi PID es: %d\n", getpid());
    printf("El PID de mi nodo padre es: %d\n", getppid());
    archivo = fopen("estructura_procesos", "w");

    if (archivo == NULL) {
        printf("No se pudo crear el archivo estructura_procesos\n");
        return;
    }

    fprintf(archivo, "Soy Nodo G, mi PID es: %d\n", getpid());
    fprintf(archivo, "El PID de mi nodo padre es: %d\n", getppid());

    fclose(archivo);

    printf("Se creo el archivo: estructura_procesos\n");
}
