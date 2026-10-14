#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void proceso_2();
void proceso_3();
void proceso_4();
void proceso_5();
void proceso_6();
void proceso_7();
void proceso_8();

// Nodo raiz: crea a P02 y P06
void proceso_1() {
    printf("[P01] PID: %d  <-  Padre: %d\n", getpid(), getppid());
    fflush(stdout);

    pid_t pid2 = fork();
    if (pid2 == 0) {
        proceso_2();
        exit(0);
    }
    wait(NULL);

    pid_t pid6 = fork();
    if (pid6 == 0) {
        proceso_6();
        exit(0);
    }
    wait(NULL);
}

// Nodo P02: crea a P03 y P05
void proceso_2() {
    printf("[P02] PID: %d  <-  Padre: %d\n", getpid(), getppid());
    fflush(stdout);

    pid_t pid3 = fork();
    if (pid3 == 0) {
        proceso_3();
        exit(0);
    }
    wait(NULL);

    pid_t pid5 = fork();
    if (pid5 == 0) {
        proceso_5();
        exit(0);
    }
    wait(NULL);
}

// Nodo P03: crea a P04
void proceso_3() {
    printf("[P03] PID: %d  <-  Padre: %d\n", getpid(), getppid());
    fflush(stdout);

    pid_t pid4 = fork();
    if (pid4 == 0) {
        proceso_4();
        exit(0);
    }
    wait(NULL);
}

// Nodo P04: hoja, no crea hijos
void proceso_4() {
    printf("[P04] PID: %d  <-  Padre: %d\n", getpid(), getppid());
    fflush(stdout);
}

// Nodo P05: hoja, no crea hijos
void proceso_5() {
    printf("[P05] PID: %d  <-  Padre: %d\n", getpid(), getppid());
    fflush(stdout);
}

// Nodo P06: crea a P07
void proceso_6() {
    printf("[P06] PID: %d  <-  Padre: %d\n", getpid(), getppid());
    fflush(stdout);

    pid_t pid7 = fork();
    if (pid7 == 0) {
        proceso_7();
        exit(0);
    }
    wait(NULL);
}

// Nodo P07: crea a P08
void proceso_7() {
    printf("[P07] PID: %d  <-  Padre: %d\n", getpid(), getppid());
    fflush(stdout);

    pid_t pid8 = fork();
    if (pid8 == 0) {
        proceso_8();
        exit(0);
    }
    wait(NULL);
}

// Nodo P08: hoja, no crea hijos
void proceso_8() {
    printf("[P08] PID: %d  <-  Padre: %d\n", getpid(), getppid());
    fflush(stdout);
}

int main() {
    proceso_1();
    return 0;
}