#include <stdio.h>
#include "sjf.h"
#include "sched_util.h"

void ejecutarSJF(ListaProcesos *listaOriginal) {
    if (!listaOriginal || listaOriginal->contador == 0) {
        printf("No hay procesos para simular.\n");
        return;
    }
    
    ListaProcesos copia;
    copiarLista(listaOriginal, &copia);
    
    printf("\n========== SIMULACION SJF (Shortest-Job-First No Preemptivo) ==========\n");
    printf("Orden de ejecucion por menor burstTime:\n\n");
    
    int tiempoActual = 0;
    int completados = 0;
    int contador = copia.contador;
    PCB *datos = copia.datos;
    
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
        trazaEntrada(tiempoActual, datos[mejorIdx].nombre, datos[mejorIdx].burstTime, 0);
        
        tiempoActual += datos[mejorIdx].burstTime;
        datos[mejorIdx].finishTime = tiempoActual;
        datos[mejorIdx].estado = ESTADO_TERMINADO;
        trazaSalida(tiempoActual, datos[mejorIdx].nombre, NULL);
        completados++;
    }
    
    mostrarResultados(datos, contador);
    listaVaciar(listaOriginal);
}
