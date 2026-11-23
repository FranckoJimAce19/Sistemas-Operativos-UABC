/**
 * @file  fcfs.c
 * @brief Simulacion FCFS (First-Come First-Served, no expropiativo).
 */
#include <stdio.h>
#include "fcfs.h"
#include "sched_util.h"

void ejecutarFCFS(PCB **listos, int contador) {
    if (!listos || contador <= 0) {
        mostrarError("fcfs", "no hay procesos para simular");
        return;
    }
    
    PCB datos[MAX_LISTOS];
    copiarListos(listos, contador, datos);
    
    iniciarSimulacion("FCFS", "First-Come, First-Served", 0, datos, contador);
    
    int tiempoActual = 0;
    
    for (int i = 0; i < contador; i++) {
        if (datos[i].arrivalTime > tiempoActual) {
            tiempoActual = datos[i].arrivalTime;
        }
        datos[i].startTime = tiempoActual;
        datos[i].estado = ESTADO_EJECUCION;
        trazaEntrada(tiempoActual, datos[i].pid, datos[i].burstTime, 0);
        
        tiempoActual += datos[i].burstTime;
        datos[i].finishTime = tiempoActual;
        datos[i].estado = ESTADO_TERMINADO;
        trazaSalida(tiempoActual, datos[i].pid, 0);
    }
    
    mostrarResultados(datos, contador);
    guardarResultados(listos, datos, contador);
}
