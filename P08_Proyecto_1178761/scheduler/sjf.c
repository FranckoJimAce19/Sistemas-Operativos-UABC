/**
 * @file  sjf.c
 * @brief Simulacion SJF (Shortest Job First, no expropiativo).
 */
#include <stdio.h>
#include "sjf.h"
#include "sched_util.h"

void ejecutarSJF(PCB **listos, int contador) {
    if (!listos || contador <= 0) {
        mostrarError("sjf", "no hay procesos para simular");
        return;
    }
    
    PCB datos[MAX_LISTOS];
    copiarListos(listos, contador, datos);
    
    iniciarSimulacion("SJF", "Shortest-Job-First", 0, datos, contador);
    
    int tiempoActual = 0;
    int completados = 0;
    
    while (completados < contador) {
        int mejorIdx = -1;
        int menorBurst = -1;
        
        for (int i = 0; i < contador; i++) {
            if (datos[i].estado == ESTADO_LISTO && datos[i].arrivalTime <= tiempoActual) {
                if (menorBurst == -1 || datos[i].burstTime < menorBurst) {
                    menorBurst = datos[i].burstTime;
                    mejorIdx = i;
                }
            }
        }
        
        /* Si no hay procesos listos al tiempoActual, avanzar el reloj */
        if (mejorIdx == -1) {
            int proxArrival = -1;
            for (int i = 0; i < contador; i++) {
                if (datos[i].estado == ESTADO_LISTO) {
                    if (proxArrival == -1 || datos[i].arrivalTime < proxArrival) {
                        proxArrival = datos[i].arrivalTime;
                    }
                }
            }
            if (proxArrival != -1 && proxArrival > tiempoActual) {
                tiempoActual = proxArrival;
            } else {
                tiempoActual++;
            }
            continue;
        }
        
        if (datos[mejorIdx].startTime == -1) {
            datos[mejorIdx].startTime = tiempoActual;
        }
        datos[mejorIdx].estado = ESTADO_EJECUCION;
        trazaEntrada(tiempoActual, datos[mejorIdx].pid, datos[mejorIdx].burstTime, 0);
        
        tiempoActual += datos[mejorIdx].burstTime;
        datos[mejorIdx].finishTime = tiempoActual;
        datos[mejorIdx].estado = ESTADO_TERMINADO;
        trazaSalida(tiempoActual, datos[mejorIdx].pid, 0);
        completados++;
    }
    
    mostrarResultados(datos, contador);
    guardarResultados(listos, datos, contador);
}
