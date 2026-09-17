#!/bin/bash

# Script: borrar_seguro.sh
# Crea una copia de un archivo antes de borrarlo

# Verificar que me dieron un argumento
if [ $# -ne 1 ]; then
    echo "Error: Debes indicar un archivo."
    echo "Uso: ./borrar_seguro.sh archivo.txt"
    exit 1
fi

ARCHIVO=$1

# Verificar que el archivo existe
if [ ! -f "$ARCHIVO" ]; then
    echo "Error: El archivo '$ARCHIVO' no existe."
    exit 1
fi

# Crear carpeta de respaldo si no existe
if [ ! -d "borrar_seguro_basura" ]; then
    mkdir borrar_seguro_basura
    echo "Carpeta 'borrar_seguro_basura' creada."
fi

# Copiar el archivo a la carpeta y borrar el original
cp "$ARCHIVO" borrar_seguro_basura/
echo "Archivo '$ARCHIVO' copiado a borrar_seguro_basura/"

rm "$ARCHIVO"
echo "Archivo '$ARCHIVO' eliminado del directorio actual."
