#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "nodos.h"

/*
 * Nodo P: es la raiz del arbol.
 * Muestra su mensaje, crea al Nodo A y espera a que termine.
 */
void nodoP(void)
{
    int pidA;

    printf("Soy el padre de todos los siguientes procesos, los cuales realizan distintas tareas\n");
    printf("El PID de mi nodo padre es: %d\n", getppid());

    fflush(stdout);

    pidA = fork();

    if (pidA == 0) {
        nodoA();
        exit(0);
    }
    waitpid(pidA, NULL, 0);

    printf("Todos los procesos han terminado\n");
}
