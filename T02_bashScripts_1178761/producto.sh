#!/bin/bash

# Script: producto.sh
# Recibe n numeros y muestra el producto de todos

# Verificar que me dieron al menos 2 numeros
if [ $# -lt 2 ]; then
    echo "Error: Debes ingresar al menos 2 numeros."
    echo "Uso: ./producto.sh 2 3 4"
    exit 1
fi

# Calcular el producto
RESULTADO=1

for NUMERO in "$@"; do
    RESULTADO=$((RESULTADO * NUMERO))
done

# Mostrar el resultado
echo "El producto de $@ es: $RESULTADO"
