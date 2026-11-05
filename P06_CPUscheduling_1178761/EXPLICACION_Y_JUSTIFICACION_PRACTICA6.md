# Explicación y Justificación — Práctica 6: CPU Scheduling

## 1. Objetivo

Implementar los algoritmos de planificación de CPU **FCFS**, **SJF no-preemptivo** y **Round-Robin** (RR, quantum = 10 ut) integrados al intérprete de comandos desarrollado desde cero en C estándar. Se organizó el código de forma modular, limpia y sin librerías externas.

---

## 2. Estructura del Proyecto

```
P06_CPUscheduling_1178761/
├── Makefile                        # Compilación con -Wall -Wextra -std=c11
├── main.c / main.h                 # Bucle del intérprete y despacho de builtins
├── process/
│   ├── pcb.h                       # PCB con campos camelCase
│   ├── lista_procesos.h            # Ready Queue estática
│   └── lista_procesos.c            # Operaciones de la lista
├── scheduler/
│   ├── sched_util.h / sched_util.c # Utilidades comunes: copiar, métricas, trazas
│   ├── fcfs.h / fcfs.c             # Algoritmo FCFS
│   ├── sjf.h  / sjf.c              # Algoritmo SJF (no preemptivo)
│   └── rr.h   / rr.c               # Algoritmo Round-Robin
├── shell_cmd/
│   ├── cmd_procesos.h              # Declaración de comandos
│   └── cmd_procesos.c              # Implementación de comandos
└── commands/                       # Comandos generales del intérprete
    ├── info.txt                    # Ayuda unificada (incluye CPU Scheduling)
    └── ...
```

**Justificación:** cada capa tiene una única responsabilidad. `process/` define tipos de datos, `scheduler/` contiene la lógica de planificación y `shell_cmd/` es la interfaz con el intérprete.

---

## 3. Convención de nombres — camelCase combinado

Se adoptó **camelCase** en todo el proyecto con el siguiente criterio:

| Categoría | Idioma | Ejemplos |
|---|---|---|
| Métricas y conceptos de SO | **Inglés, camelCase** | `burstTime`, `arrivalTime`, `startTime`, `finishTime`, `waitingTime`, `turnaroundTime`, `remainingTime` |
| Variables estructurales / de control | **Español, camelCase** | `datos`, `contador`, `siguientePid`, `tiempoActual`, `enCola`, `frente`, `final`, `completados`, `mejorIdx` |
| Funciones y comandos | **camelCase** | `ejecutarFCFS`, `ejecutarSJF`, `ejecutarRR`, `cmdFcfs`, `cmdSjf`, `cmdRr`, `copiarLista`, `mostrarResultados` |
| Nombres de proceso y estado | **Español** | `nombre`, `estado`, `EstadoProceso` |

**Justificación:** los términos de teoría de Sistemas Operativos (*burst time*, *waiting time*, *turnaround*, *arrival time*) tienen un significado estándar en inglés y se mantienen así para coincidir con la bibliografía (Silberschatz). Las variables de almacenamiento y control se nombran en español para legibilidad inmediata.

---

## 4. PCB — Bloque de Control de Proceso (`process/pcb.h`)

```c
typedef struct {
    int pid;                        /* Identificador único */
    char nombre[MAX_PROC_NAME];     /* Nombre del proceso (español) */
    int burstTime;                  /* CPU burst time (inglés) */
    int remainingTime;              /* Burst restante para RR (inglés) */
    int arrivalTime;                /* Tiempo de llegada (inglés) */
    int startTime;                  /* Primer acceso a CPU (inglés) */
    int finishTime;                 /* Tiempo de finalización (inglés) */
    int waitingTime;                /* Waiting time = Turnaround - Burst */
    int turnaroundTime;             /* Turnaround time = Finish - Arrival */
    EstadoProceso estado;           /* Estado actual (español) */
} PCB;
```

El archivo incluye macros de alias para compatibilidad con cualquier nombre anterior (`burst_time`, `rafaga`, `tiempoInicio`, etc.), sin duplicar memoria.

---

## 5. Lista de Procesos (`process/lista_procesos`)

Implementación con **arreglo estático** de hasta 256 procesos. Se eligió arreglo estático porque:
- Es suficiente para la práctica.
- Evita `malloc`/`free` y posibles memory leaks.
- Simplifica la lógica de copia para los algoritmos.

```c
typedef struct {
    PCB datos[MAX_LISTA];   /* 'datos' en español */
    int contador;           /* 'contador' en español */
    int siguientePid;       /* PID auto-incremental */
} ListaProcesos;
```

---

## 6. Utilidades Comunes (`scheduler/sched_util`)

Centraliza la lógica compartida por los tres algoritmos:

| Función | Propósito |
|---|---|
| `copiarLista(origen, destino)` | Copia profunda reseteando métricas, para simular desde cero sin alterar la lista original. |
| `resetearProcesos(lista)` | Reinicia tiempos y estado sin copiar la estructura. |
| `calcularMetricas(datos, contador)` | `turnaroundTime = finishTime - arrivalTime` / `waitingTime = turnaroundTime - burstTime` |
| `mostrarResultados(datos, contador)` | Tabla formateada con promedios de Waiting y Turnaround Time. |
| `trazaEntrada(tiempoActual, nombre, ...)` | Traza uniforme de entrada a CPU. |
| `trazaSalida(tiempoActual, nombre, detalle)` | Traza uniforme de salida de CPU. |

**Justificación:** evita duplicación de código entre los tres algoritmos. Cualquier algoritmo nuevo solo necesita llamar a estas utilidades.

---

## 7. Algoritmos Implementados

### 7.1 FCFS (`scheduler/fcfs.c`) — `ejecutarFCFS`

**Lógica:** FIFO estricto no-preemptivo. Ejecuta cada proceso en orden de llegada.

```
tiempoActual = 0
Para cada proceso en orden:
    Si arrivalTime > tiempoActual → avanzar reloj
    Ejecutar burstTime completo
    Registrar finishTime
```

**Fórmulas:**
- `waitingTime = (tiempoActual antes de ejecutar) - arrivalTime`
- `turnaroundTime = finishTime - arrivalTime`

---

### 7.2 SJF No Preemptivo (`scheduler/sjf.c`) — `ejecutarSJF`

**Lógica:** selecciona siempre el proceso listo con menor `burstTime`. Si hay empate, elige el primero encontrado (FCFS como desempate).

```
Mientras haya procesos sin completar:
    Buscar el proceso LISTO con menor burstTime que ya haya llegado
    Si ninguno está listo → saltar al próximo arrivalTime
    Ejecutar burstTime completo
```

**Optimización:** cuando la CPU queda ociosa, el reloj salta directamente al siguiente `arrivalTime` en vez de incrementar de 1 en 1.

---

### 7.3 Round-Robin (`scheduler/rr.c`) — `ejecutarRR`

**Quantum = 10 ut** (`QUANTUM_RR` definido en `rr.h`).

**Lógica:** cola circular con índices (`int cola[MAX_COLA_RR]`), sin mover elementos en memoria.

```
Encolar todos los procesos
Mientras haya procesos sin terminar:
    Sacar el frente de cola
    Si ya terminó → ignorar
    Si aún no llegó → re-encolar, avanzar reloj
    Calcular ejecucion = min(remainingTime, QUANTUM_RR)
    Ejecutar ejecucion unidades
    Si remainingTime == 0 → TERMINADO
    Si no → re-encolar (vuelve a cola)
```

**Variables de la cola:**
```c
int cola[MAX_COLA_RR];       /* Índices de procesos */
int frente = 0, final = 0;  /* Punteros circulares */
int enCola = 0;             /* Cantidad en cola */
```

**Justificación del tamaño `MAX_COLA_RR = 1024`:** en el peor caso un proceso de burst B con quantum Q genera ⌈B/Q⌉ entradas en la cola. Para 256 procesos con burst máximo razonable esto es más que suficiente y evita desbordamientos.

---

## 8. Comandos del Intérprete

| Comando | Función | Alias aceptado |
|---|---|---|
| `mkprocess [nombre] [burstTime]` | Agrega proceso a la lista | `crearProc` |
| `lstprocss` | Muestra los procesos en la lista | `mostrarProc` |
| `fcfs` | Simula FCFS | `planFCFS` |
| `sjf` | Simula SJF | `planSJF` |
| `rr` | Simula Round-Robin (quantum=10) | `planRR` |

Implementados en [`shell_cmd/cmd_procesos.c`](shell_cmd/cmd_procesos.c) como funciones camelCase:
`cmdMkprocess`, `cmdLstprocss`, `cmdFcfs`, `cmdSjf`, `cmdRr`.

**Comportamiento tras simular:** la lista de procesos se vacía automáticamente con `listaVaciar()`, según lo exige la práctica.

---

## 9. Integración al Intérprete (`main.c`)

- `isBuiltin()` reconoce los cinco comandos de scheduling y sus alias.
- `runBuiltin()` los despacha a las funciones `cmd*` correspondientes.
- La lista global `listaProcesosGlobal` se inicializa en `main()` con `listaInicializar()`.
- Los comandos sin tubería (`|`) que sean builtins se ejecutan en el proceso padre, preservando el estado de la lista entre comandos.

---

## 10. Decisiones de Diseño

| Decisión | Justificación |
|---|---|
| camelCase en todo el proyecto | Consistencia visual. Distingue variables locales (`tiempoActual`) de campos PCB (`burstTime`) de constantes (`QUANTUM_RR`). |
| Métricas en inglés | Coinciden con la terminología de Silberschatz y la especificación de la práctica. |
| Variables estructurales en español | Legibilidad inmediata (`datos`, `contador`, `frente`, `final`). |
| Arreglo estático en `ListaProcesos` | Sin `malloc`/`free`. Simple, seguro, suficiente para la práctica. |
| Cola circular con índices en RR | Evita mover datos en memoria. Eficiente y sin desbordamiento gracias a `MAX_COLA_RR`. |
| `copiarLista()` antes de simular | Los algoritmos operan sobre una copia, la lista original solo se vacía al terminar. |
| Alias de compatibilidad en headers | Permiten usar `burst_time`, `rafaga`, etc. sin romper código existente. |
| Un solo `sched_util` | Centraliza métricas y trazas. Todos los algoritmos producen la misma tabla de resultados. |
| Sin librerías externas | Solo libc estándar (`stdio.h`, `stdlib.h`, `string.h`, `unistd.h`, `signal.h`). |

---

## 11. Cumplimiento de Requisitos

| Requisito de la Práctica | Estado |
|---|---|
| `mkprocess [nombre] [burst_time]` guarda en lista | ✅ `cmdMkprocess` → `listaAgregar` |
| `lstprocss` muestra procesos | ✅ `cmdLstprocss` → `listaMostrar` |
| FCFS implementado | ✅ `ejecutarFCFS` |
| SJF implementado | ✅ `ejecutarSJF` |
| Round-Robin con quantum = 10 ut | ✅ `ejecutarRR`, `QUANTUM_RR 10` |
| Waiting y turnaround por proceso y promedio | ✅ `calcularMetricas` + `mostrarResultados` |
| Trazas de entrada/salida de CPU con tiempo | ✅ `trazaEntrada` / `trazaSalida` |
| Vaciar lista tras simular | ✅ `listaVaciar` al final de cada algoritmo |
| Sin librerías externas | ✅ Solo libc estándar |
| Código modular sin repetición | ✅ `sched_util` centraliza la lógica común |

---

## 12. Compilar y Probar

```bash
make clean
make
./interprete
```

### Ejemplo de sesión

```text
so@interprete:~ >>> info
so@interprete:~ >>> mkprocess P1 24
so@interprete:~ >>> mkprocess P2 3
so@interprete:~ >>> mkprocess P3 3
so@interprete:~ >>> lstprocss
so@interprete:~ >>> fcfs

so@interprete:~ >>> mkprocess A 6
so@interprete:~ >>> mkprocess B 8
so@interprete:~ >>> mkprocess C 7
so@interprete:~ >>> mkprocess D 3
so@interprete:~ >>> sjf

so@interprete:~ >>> mkprocess X 30
so@interprete:~ >>> mkprocess Y 15
so@interprete:~ >>> rr
```

> **Nota:** tras cada simulación la lista se vacía automáticamente.

---

## 13. Referencias

- Silberschatz, A., Galvin, P., Gagne, G. *Operating System Concepts*, 10th ed. John Wiley & Sons.
