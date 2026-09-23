#!/bin/bash

LISTA="$1"

if [ -z "$LISTA" ]; then
    echo "Uso: sudo $0 <archivo_lista_alumnos>"
    exit 1
fi

if [ ! -f "$LISTA" ]; then
    echo "Error: el archivo '$LISTA' no existe."
    exit 1
fi

# Debe ejecutarse como root para eliminar usuarios
if [ "$(id -u)" -ne 0 ]; then
    echo "Error: debes ejecutar con sudo (se necesita root para eliminar usuarios)."
    exit 1
fi

# Quita tildes y pasa todo a minusculas (igual que el script de creacion)
normalizar() {
    echo "$1" \
        | sed -e 's/á/a/g; s/é/e/g; s/í/i/g; s/ó/o/g; s/ú/u/g; s/ü/u/g; s/ñ/n/g' \
        | tr 'A-Z' 'a-z' \
        | tr -cd 'a-z0-9'
}

total=0

# Leemos cada linea del archivo
while IFS= read -r linea; do
    [ -z "$linea" ] && continue

    apellido=$(normalizar "$(echo "$linea" | awk '{print $2}')")
    nombre=$(normalizar "$(echo "$linea" | awk '{print $1}')")

    [ -z "$apellido" ] && [ -z "$nombre" ] && continue

    usuario="$apellido.$nombre"

    # Seguridad: nunca eliminar root ni el propio usuario
    if [ "$usuario" = "root" ] || [ "$usuario" = "$USER" ]; then
        echo "SEGURIDAD: '$usuario' no se elimina por ser cuenta critica."
        continue
    fi

    # Si el usuario no existe no hay nada que eliminar
    if ! id "$usuario" >/dev/null 2>&1; then
        echo "El usuario '$usuario' no existe, se omite."
        continue
    fi

    # userdel -r elimina la cuenta y su carpeta personal
    if userdel -r "$usuario" 2>/dev/null; then
        echo "Usuario eliminado: $usuario"
        total=$((total + 1))
    else
        echo "ERROR: no se pudo eliminar '$usuario'."
    fi
done < <(cat "$LISTA"; echo)

echo "-----------------------------------------"
echo "Total de usuarios eliminados: $total"