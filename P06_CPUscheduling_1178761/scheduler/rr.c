#include <stdio.h>
#include "rr.h"
#include "sched_util.h"

#define MAX_COLA_RR 1024

void ejecutarRR(ListaProcesos *listaOriginal) {
    if (!listaOriginal || listaOriginal->contador == 0) {
        printf("No hay procesos para simular.\n");
        return;
    }
    
    ListaProcesos copia;
    copiarLista(listaOriginal, &copia);
    
    printf("\n========== SIMULACION ROUND-ROBIN ==========\n");
    printf("Quantum = %d ut\n\n", QUANTUM_RR);
    
    int tiempoActual = 0;
    int completados = 0;
    int contador = copia.contador;
    PCB *datos = copia.datos;
    
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

        trazaEntrada(tiempoActual, datos[idx].nombre, datos[idx].remainingTime, ejecucion);
        tiempoActual += ejecucion;
        datos[idx].remainingTime -= ejecucion;
        
        if (datos[idx].remainingTime == 0) {
            datos[idx].finishTime = tiempoActual;
            datos[idx].estado = ESTADO_TERMINADO;
            trazaSalida(tiempoActual, datos[idx].nombre, "TERMINADO");
            completados++;
        } else {
            datos[idx].estado = ESTADO_LISTO;
            trazaSalida(tiempoActual, datos[idx].nombre, "vuelve a cola");
            cola[final] = idx;
            final = (final + 1) % MAX_COLA_RR;
            enCola++;
        }
    }
    
    mostrarResultados(datos, contador);
    listaVaciar(listaOriginal);
}
