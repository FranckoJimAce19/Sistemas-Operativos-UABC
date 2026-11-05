# **1\. INTRODUCCIÓN**

El objetivo de la multiprogramación es que un proceso siempre en ejecución así como maximizar la utilización de CPU. Un núcleo del CPU debe cambiar entre procesos de forma frecuente de tal forma que el usuario pueda interactuar con cada programa mientras se encuentra en ejecución. Para esto, los sistemas operativos cuentan con el CPU scheduler, el cual tiene la tarea de elegir qué proceso utilizará el CPU de acuerdo a un algoritmo de calendarización.

# **2\. OBJETIVO (COMPETENCIA)** 

Implementar los algoritmos de las técnicas de calendarización de procesos en CPU mediante la elaboración de un programa para comprender el funcionamiento de los mismos, mostrando actitud crítica y disposición por el estudio.

 

# **3\. FUNDAMENTO**

**First-Come, First-Served**  
El primer proceso que solicite primero el CPU  es al que se le asigna. Su implementación es fácilmente manejada por una estructura FIFO: cuando un CPU se encuentre disponible, a este se le asignará el proceso que se encuentre al inicio de la FIFO. Si un nuevo proceso llega a la ready queue, este se encontrará al final de la cola.

**Shortest-Job-First**  
Cuando un CPU está disponible, este es asignado al proceso con el menor CPU burst. Si llega a haber más de un proceso con un mismo tiempo, se utiliza FCFS para el desempate.   
La calendarización de este algoritmo depende de la duración del CPU burst del siguiente proceso a ser asignado al procesador.

**Round-Robin**  
Presenta un funcionamiento similar a FCFS, pero permite al sistema cambiar entre procesos (preemption). Se define una unidad de tiempo llamada time quantum y la ready queue es tratada como una cola circular. El CPU scheduler asigna un proceso a un CPU en un intervalo de tiempo igual a 1 time quantum.  
	El CPU scheduler selecciona el primer proceso de la ready queue, define un temporizador para interrumpir al proceso después de un time quantum y aloja al proceso. 

* Si el burst time del proceso es menor a 1 time quantum, este liberará al CPU voluntariamente.   
* De ser mayor, se hace un context switch y el proceso se coloca al final de la ready queue.

| 4\. PROCEDIMIENTO   |  |
| ----- | ----- |
| **Equipo Necesario** | **Material de Apoyo** |
| Computadora con sistema operativo Ubuntu instalado. | Formato de Práctica. |

# **Desarrollo de la Práctica** 

Realice un programa en C que funcione como una terminal de comandos la cual permite crear, eliminar y simular los de *CPU Scheduling*. Los 3 algoritmos a implementar son:

* First-Come, first-served (FCFS)  
* Shortest-job-first (SJF)   
* Round-Robin (RR)

El usuario en la terminal podrá crear un proceso, el cual se almacenará en una lista (equivalente a la ready queue) por medio del comando *mkprocess*, seguido de un nombre que identifique al proceso y su burst time. A su vez, realizarán la programación de un comando, *lstprocss* el cual mostrará todos los procesos en la lista.   
Se programaran 3 comandos, uno para cada algoritmo, que permitan simular la calendarización de procesos, definiendo un time quantum para RR igual a 10 ut. Para cada algoritmo se deberá calcular el *waiting* y *turnaround time* por proceso y promedio, con la finalidad de reportar dichas métricas en una tabla. Adicionalmente, cada que un proceso libere o sea asignado al CPU deberá mostrarse en pantalla el nombre del proceso que entra y sale del CPU, en conjunto con el tiempo de calendarización. Cada que uno de estos algoritmos se ejecute, la lista de procesos se vaciará. 

5. **RESULTADOS Y CONCLUSIONES**

Como resultado de esta práctica se enviará un reporte de práctica el cual deberá contener portada, una breve introducción, el desarrollo de las instrucciones planteadas en el procedimiento, una breve conclusión y adjuntar el código en una sección de anexos. 

# **6\. REFERENCIAS** 

* Silberschatz, P. Galvin and G. Gagne, Operating system concepts, 10th ed. John Wiley & Sons, Inc. 


