#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    int pidHijo;
    int i;

    pidHijo = fork();

    if (pidHijo == 0) {
        printf("[HIJO]  PID: %d | Termino y me quedo como zombie\n", getpid());
        fflush(stdout);
        exit(0);
    }

    printf("[PADRE] PID: %d | Cree al hijo (zombie) PID: %d\n", getpid(), pidHijo);
    fflush(stdout);

    //Tiempo de espera para el padre
    for (i = 0; i < 65; i++) {
        sleep(1);
    }

    waitpid(pidHijo, NULL, 0);
    printf("Se murio el zombie \n");
    fflush(stdout);

    return 0;
}
