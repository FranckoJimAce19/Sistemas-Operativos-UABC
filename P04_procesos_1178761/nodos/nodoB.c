#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "nodos.h"

/*
 * Nodo B: muestra su mensaje y el nombre del directorio actual.
 */
void nodoB(void)
{
    printf("Soy Nodo B, mi PID es: %d\n", getpid());
    printf("El PID de mi nodo padre es: %d\n", getppid());

    system("pwd");
}
