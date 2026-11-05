#include <stdio.h>
#include "fcfs.h"
#include "sched_util.h"

void ejecutarFCFS(ListaProcesos *listaOriginal) {
    if (!listaOriginal || listaOriginal->contador == 0) {
        printf("No hay procesos para simular.\n");
        return;
    }
    
    ListaProcesos copia;
    copiarLista(listaOriginal, &copia);
    
    printf("\n========== SIMULACION FCFS (First-Come First-Served) ==========\n");
    printf("Orden de ejecucion (FIFO no-preemptivo):\n\n");
    
    int tiempoActual = 0;
    int contador = copia.contador;
    PCB *datos = copia.datos;
    
    for (int i = 0; i < contador; i++) {
        if (datos[i].arrivalTime > tiempoActual) {
            tiempoActual = datos[i].arrivalTime;
        }
        datos[i].startTime = tiempoActual;
        datos[i].estado = ESTADO_EJECUCION;
        trazaEntrada(tiempoActual, datos[i].nombre, datos[i].burstTime, 0);
        
        tiempoActual += datos[i].burstTime;
        datos[i].finishTime = tiempoActual;
        datos[i].estado = ESTADO_TERMINADO;
        trazaSalida(tiempoActual, datos[i].nombre, NULL);
    }
    
    mostrarResultados(datos, contador);
    listaVaciar(listaOriginal);
}
