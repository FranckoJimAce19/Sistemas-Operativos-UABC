#!/bin/bash

# Script: verificar_contrasena.sh
# Verifica si una contrasena es segura

# Verificar que me dieron un argumento
if [ $# -ne 1 ]; then
    echo "Error: Debes indicar una contrasena."
    echo "Uso: ./verificar_contrasena.sh MiContrasena123"
    exit 1
fi

CONTRASENA=$1
SEGURO=1

# Verificar longitud minima de 8 caracteres
if [ ${#CONTRASENA} -lt 8 ]; then
    echo "No cumple: Debe tener al menos 8 caracteres."
    SEGURO=0
fi

# Verificar que tenga letras y numeros
if ! echo "$CONTRASENA" | grep -q "[a-zA-Z]"; then
    echo "No cumple: Debe tener letras."
    SEGURO=0
fi

if ! echo "$CONTRASENA" | grep -q "[0-9]"; then
    echo "No cumple: Debe tener numeros."
    SEGURO=0
fi

# Verificar que tenga mayusculas y minusculas
if ! echo "$CONTRASENA" | grep -q "[A-Z]"; then
    echo "No cumple: Debe tener letras mayusculas."
    SEGURO=0
fi

if ! echo "$CONTRASENA" | grep -q "[a-z]"; then
    echo "No cumple: Debe tener letras minusculas."
    SEGURO=0
fi

# 5. Mostrar resultado final
if [ $SEGURO -eq 1 ]; then
    echo "Contrasena SEGURA."
fi
