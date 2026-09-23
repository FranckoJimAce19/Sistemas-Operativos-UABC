#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Uso: $0 <directorio_a_respaldar> <directorio_destino>"
    exit 1
fi

ORIGEN="${1%/}"
DESTINO="${2%/}"

# El origen debe ser un directorio
if [ ! -d "$ORIGEN" ]; then
    echo "Error: '$ORIGEN' no es un directorio valido."
    exit 1
fi

if [ "$DESTINO" = "$ORIGEN" ] || [ "${DESTINO#$ORIGEN/}" != "$DESTINO" ]; then
    echo "Error: el destino '$DESTINO' no puede ser el origen ni estar dentro de el."
    exit 1
fi

# Contamos todos los archivos a respaldar (directos, de subcarpetas y enlaces)
total=$(find "$ORIGEN" \( -type f -o -type l \) | wc -l)
fecha=$(date +"%Y-%m-%d %H:%M:%S")

# Creamos el destino si no existe
mkdir -p "$DESTINO"

# Copiamos todos los archivos y subdirectorios
if ! cp -r "$ORIGEN"/. "$DESTINO"/; then
    echo "Error: fallo la copia en '$DESTINO'."
    exit 1
fi

# Creamos el README con la fecha y el total de archivos
printf "RESPALDO\nFecha: %s\nTotal de archivos respaldados: %d\n" "$fecha" "$total" > "$DESTINO/README"

echo "Respaldo completado."
echo "Directorio destino: $DESTINO"
echo "Fecha del respaldo: $fecha"
echo "Total de archivos respaldados: $total"