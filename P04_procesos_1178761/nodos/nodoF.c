#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "nodos.h"

/*
 * Nodo F: muestra su mensaje, lista los procesos actuales.
 */
void nodoF(void)
{
    int pidG;

    printf("Soy Nodo F, mi PID es: %d\n", getpid());
    printf("El PID de mi nodo padre es: %d\n", getppid());

    system("ps -l");

    fflush(stdout);

    // Creamos al Nodo G
    pidG = fork();

    if (pidG == 0) {
        nodoG();
        exit(0);
    }

    // Esperamos a que G termine
    waitpid(pidG, NULL, 0);
}
