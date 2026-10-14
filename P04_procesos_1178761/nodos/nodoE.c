#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "nodos.h"

/*
 * Nodo E: muestra su mensaje y la fecha del sistema.
 */
void nodoE(void)
{
    printf("Soy Nodo E, mi PID es: %d\n", getpid());
    printf("El PID de mi nodo padre es: %d\n", getppid());

    system("date");
}
