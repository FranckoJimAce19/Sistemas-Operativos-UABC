#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#include "nodos.h"

static void esperarTecla(void)
{
    struct termios attrsViejos;
    struct termios attrsNuevos;
    int esTerminal;

    printf("\nPresiona cualquier tecla para limpiar la pantalla...");
    fflush(stdout);

    esTerminal = isatty(STDIN_FILENO);

    if (esTerminal && tcgetattr(STDIN_FILENO, &attrsViejos) == 0) {
        attrsNuevos = attrsViejos;
        attrsNuevos.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &attrsNuevos);
        getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &attrsViejos);
    } else {
        getchar();
    }

    printf("\n");
}

/*
 * Nodo A: crea a los Nodos B y C. Tambien ejecuta clear.
 */
void nodoA(void)
{
    int pidB;
    int pidC;

    printf("Soy Nodo A y mi PID es: %d\n", getpid());
    printf("El PID de mi nodo padre es: %d\n", getppid());

    fflush(stdout);

    // Creamos al Nodo B
    pidB = fork();

    if (pidB == 0) {
        nodoB();
        exit(0);
    }

    // Creamos al Nodo C
    pidC = fork();

    if (pidC == 0) {
        nodoC();
        exit(0);
    }

    // Esperamos a que B y C terminen
    waitpid(pidB, NULL, 0);
    waitpid(pidC, NULL, 0);

    // Clear se ejecuta cuando ya se mostro todo en pantalla y el usuario lo confirma
    esperarTecla();
    system("clear");
}
