/**
 * @file  rr.c
 * @brief Simulacion Round-Robin con quantum QUANTUM_RR.
 */
#include <stdio.h>
#include "rr.h"
#include "sched_util.h"

#define MAX_COLA_RR 1024

void ejecutarRR(PCB **listos, int contador) {
    if (!listos || contador <= 0) {
        mostrarError("rr", "no hay procesos para simular");
        return;
    }
    
    PCB datos[MAX_LISTOS];
    copiarListos(listos, contador, datos);
    
    iniciarSimulacion("RR", "Round-Robin", QUANTUM_RR, datos, contador);
    
    int tiempoActual = 0;
    int completados = 0;
    
    /* Cola circular segura de indices */
    int cola[MAX_COLA_RR];
    int frente = 0, final = 0, enCola = 0;
    
    /* Encolar todos los procesos iniciales */
    for (int i = 0; i < contador; i++) {
        cola[final] = i;
        final = (final + 1) % MAX_COLA_RR;
        enCola++;
    }
    
    while (completados < contador) {
        if (enCola == 0) {
            tiempoActual++;
            continue;
        }
        
        int idx = cola[frente];
        frente = (frente + 1) % MAX_COLA_RR;
        enCola--;
        
        if (datos[idx].estado == ESTADO_TERMINADO) {
            continue;
        }
        
        if (datos[idx].arrivalTime > tiempoActual) {
            /* Re-encolar y esperar a que llegue */
            cola[final] = idx;
            final = (final + 1) % MAX_COLA_RR;
            enCola++;
            tiempoActual++;
            continue;
        }
        
        if (datos[idx].startTime == -1) {
            datos[idx].startTime = tiempoActual;
        }
        datos[idx].estado = ESTADO_EJECUCION;
        
        int ejecucion = (datos[idx].remainingTime < QUANTUM_RR)
                            ? datos[idx].remainingTime
                            : QUANTUM_RR;

        trazaEntrada(tiempoActual, datos[idx].pid, datos[idx].remainingTime, ejecucion);
        tiempoActual += ejecucion;
        datos[idx].remainingTime -= ejecucion;
        
        if (datos[idx].remainingTime == 0) {
            datos[idx].finishTime = tiempoActual;
            datos[idx].estado = ESTADO_TERMINADO;
            trazaSalida(tiempoActual, datos[idx].pid, 0);
            completados++;
        } else {
            datos[idx].estado = ESTADO_LISTO;
            trazaSalida(tiempoActual, datos[idx].pid, datos[idx].remainingTime);
            cola[final] = idx;
            final = (final + 1) % MAX_COLA_RR;
            enCola++;
        }
    }
    
    mostrarResultados(datos, contador);
    guardarResultados(listos, datos, contador);
}
