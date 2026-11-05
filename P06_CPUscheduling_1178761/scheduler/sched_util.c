#include <stdio.h>
#include <string.h>
#include "sched_util.h"

void copiarLista(const ListaProcesos *origen, ListaProcesos *destino) {
    if (!origen || !destino) return;
    destino->contador = origen->contador;
    destino->siguientePid = origen->siguientePid;
    for (int i = 0; i < origen->contador; i++) {
        destino->datos[i] = origen->datos[i];
        destino->datos[i].remainingTime = destino->datos[i].burstTime;
        destino->datos[i].startTime = -1;
        destino->datos[i].finishTime = -1;
        destino->datos[i].waitingTime = 0;
        destino->datos[i].turnaroundTime = 0;
        destino->datos[i].estado = ESTADO_LISTO;
    }
}

void resetearProcesos(ListaProcesos *lista) {
    if (!lista) return;
    for (int i = 0; i < lista->contador; i++) {
        lista->datos[i].remainingTime = lista->datos[i].burstTime;
        lista->datos[i].startTime = -1;
        lista->datos[i].finishTime = -1;
        lista->datos[i].waitingTime = 0;
        lista->datos[i].turnaroundTime = 0;
        lista->datos[i].estado = ESTADO_LISTO;
    }
}

void calcularMetricas(PCB *datos, int contador) {
    if (!datos || contador <= 0) return;
    for (int i = 0; i < contador; i++) {
        if (datos[i].finishTime >= 0 && datos[i].arrivalTime >= 0) {
            datos[i].turnaroundTime = datos[i].finishTime - datos[i].arrivalTime;
        }
        if (datos[i].turnaroundTime >= 0 && datos[i].burstTime >= 0) {
            datos[i].waitingTime = datos[i].turnaroundTime - datos[i].burstTime;
            if (datos[i].waitingTime < 0) {
                datos[i].waitingTime = 0;
            }
        }
    }
}

void trazaEntrada(int tiempoActual, const char *nombre, int burstORestante, int quantum) {
    if (quantum > 0) {
        printf("[Tiempo %d] Entra a CPU: %s (remaining=%d, quantum=%d)\n",
               tiempoActual, nombre, burstORestante, quantum);
    } else {
        printf("[Tiempo %d] Entra a CPU: %s (burstTime=%d)\n",
               tiempoActual, nombre, burstORestante);
    }
}

void trazaSalida(int tiempoActual, const char *nombre, const char *detalle) {
    if (detalle && detalle[0] != '\0') {
        printf("[Tiempo %d] Sale de CPU:  %s (%s)\n", tiempoActual, nombre, detalle);
    } else {
        printf("[Tiempo %d] Sale de CPU:  %s\n", tiempoActual, nombre);
    }
}

void mostrarResultados(PCB *datos, int contador) {
    if (!datos || contador <= 0) {
        printf("No hay procesos para mostrar resultados.\n");
        return;
    }
    calcularMetricas(datos, contador);
    
    printf("\n==================== RESULTADOS DE SIMULACION ====================\n");
    printf("%-6s %-16s %-12s %-14s %-14s %-12s\n", 
           "PID", "PROCESO", "BURST TIME", "WAITING TIME", "TURNAROUND", "FINISH TIME");
    printf("--------------------------------------------------------------------------------\n");
    
    float sumaWaiting = 0.0f;
    float sumaTurnaround = 0.0f;
    for (int i = 0; i < contador; i++) {
        printf("%-6d %-16s %-12d %-14d %-14d %-12d\n", 
               datos[i].pid, 
               datos[i].nombre, 
               datos[i].burstTime,
               datos[i].waitingTime,
               datos[i].turnaroundTime,
               datos[i].finishTime);
        sumaWaiting += (float)datos[i].waitingTime;
        sumaTurnaround += (float)datos[i].turnaroundTime;
    }
    printf("--------------------------------------------------------------------------------\n");
    printf("Promedio Waiting Time:    %.2f ut\n", sumaWaiting / (float)contador);
    printf("Promedio Turnaround Time: %.2f ut\n", sumaTurnaround / (float)contador);
    printf("================================================================================\n\n");
}
