#!/bin/bash

# Script: contar_palabras.sh
# Muestra todas las palabras del archivo y cuantas veces aparecen

# Verificar que me dieron un argumento
if [ $# -ne 1 ]; then
    echo "Error: Debes indicar un archivo."
    echo "Uso: ./contar_palabras.sh archivo.txt"
    exit 1
fi

ARCHIVO=$1

# Verificar que el archivo existe
if [ ! -f "$ARCHIVO" ]; then
    echo "Error: El archivo '$ARCHIVO' no existe."
    exit 1
fi

# Contar y mostrar las palabras
echo "Palabra          Veces"
echo "---------------------"
awk '{for(i=1;i<=NF;i++) print $i}' "$ARCHIVO" | sort | uniq -c | sort -rn | awk '{printf "%-18s %s\n", $2, $1}'
