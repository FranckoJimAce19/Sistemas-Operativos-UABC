#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "nodos.h"

/*
 * Nodo C: muestra su mensaje, el contenido del directorio actual.
 */
void nodoC(void)
{
    int pidD;

    printf("Soy Nodo C, mi PID es: %d y el PID de mi nodo padre es: %d\n", getpid(), getppid());

    system("ls -l");

    fflush(stdout);

    // Creamos al Nodo D
    pidD = fork();

    if (pidD == 0) {
        nodoD();
        exit(0);
    }

    // Esperamos a que D termine
    waitpid(pidD, NULL, 0);
}
