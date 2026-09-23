#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Uso: $0 <archivo-o-directorio>"
    exit 1
fi

RUTA="$1"

# Comprobamos el tipo de elemento
if [ ! -e "$RUTA" ] && [ ! -L "$RUTA" ]; then
    echo "'$RUTA' no existe."
elif [ -f "$RUTA" ]; then
    echo "'$RUTA' es un archivo."
elif [ -d "$RUTA" ]; then
    echo "'$RUTA' es un directorio."
elif [ -L "$RUTA" ]; then
    echo "'$RUTA' es un enlace simbolico."
else
    echo "'$RUTA' es otro tipo de elemento."
fi