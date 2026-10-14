#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "nodos.h"

/*
 * Nodo D: muestra su mensaje, lista los procesos actuales.
 */
void nodoD(void)
{
    int pidE;
    int pidF;

    printf("Soy Nodo D, mi PID es: %d\n", getpid());
    printf("El PID de mi nodo padre es: %d\n", getppid());

    system("ps -l");

    fflush(stdout);

    // Creamos al Nodo E
    pidE = fork();

    if (pidE == 0) {
        nodoE();
        exit(0);
    }

    // Creamos al Nodo F
    pidF = fork();

    if (pidF == 0) {
        nodoF();
        exit(0);
    }

    // Esperamos a que E y F terminen
    waitpid(pidE, NULL, 0);
    waitpid(pidF, NULL, 0);
}
