#!/bin/bash

# Script: borrar_palabra.sh
# Elimina todas las lineas que contengan una palabra de un archivo

# Verificar que me dieron 2 argumentos
if [ $# -ne 2 ]; then
    echo "Error: Debes indicar una palabra y un archivo."
    echo "Uso: ./borrar_palabra.sh palabra archivo.txt"
    exit 1
fi

PALABRA=$1
ARCHIVO=$2

# Verificar que el archivo existe
if [ ! -f "$ARCHIVO" ]; then
    echo "Error: El archivo '$ARCHIVO' no existe."
    exit 1
fi

# Eliminar las lineas que contengan la palabra
sed -i "/$PALABRA/d" "$ARCHIVO"
echo "Se eliminaron las lineas con '$PALABRA' del archivo '$ARCHIVO'."
