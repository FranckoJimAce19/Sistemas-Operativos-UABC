CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11

SRCS    = arbol.c nodos/nodoP.c nodos/nodoA.c nodos/nodoB.c nodos/nodoC.c nodos/nodoD.c nodos/nodoE.c nodos/nodoF.c nodos/nodoG.c
ZSRC    = zombie.c

.PHONY: all arbol zombie clean

# make all: compila los dos programas
all: arbol zombie

# make arbol: compila el arbol de procesos
arbol:
	$(CC) $(CFLAGS) -o arbol $(SRCS)
	@echo "Compilacion exitosa: ./arbol"

# make zombie: compila el programa zombie
zombie:
	$(CC) $(CFLAGS) -o zombie $(ZSRC)
	@echo "Compilacion exitosa: ./zombie"

# make clean: elimina los binarios y el archivo generado
clean:
	rm -f arbol zombie estructura_procesos
	@echo "Binarios y archivo estructura_procesos eliminados."
