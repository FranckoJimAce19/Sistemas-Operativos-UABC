#!/bin/bash

# Parametros: lista de alumnos y archivo donde guardar las cuentas
LISTA="$1"
ARCHIVO_CUENTAS="${2:-cuentas_creadas.txt}"

# Validaciones de uso
if [ -z "$LISTA" ]; then
    echo "Uso: sudo $0 <archivo_lista_alumnos> [archivo_cuentas]"
    exit 1
fi

if [ ! -f "$LISTA" ]; then
    echo "Error: el archivo '$LISTA' no existe."
    exit 1
fi

# Debe ejecutarse como root para crear usuarios
if [ "$(id -u)" -ne 0 ]; then
    echo "Error: debes ejecutar con sudo (se necesita root para crear usuarios)."
    exit 1
fi

# Quita tildes y pasa todo a minusculas para formar el usuario
normalizar() {
    echo "$1" \
        | sed -e 's/á/a/g; s/é/e/g; s/í/i/g; s/ó/o/g; s/ú/u/g; s/ü/u/g; s/ñ/n/g' \
        | tr 'A-Z' 'a-z' \
        | tr -cd 'a-z0-9'
}

total=0
contenido=""

# Leemos cada linea del archivo
while IFS= read -r linea; do
    [ -z "$linea" ] && continue

    # El apellido forma el usuario y la password
    apellido=$(normalizar "$(echo "$linea" | awk '{print $2}')")
    nombre=$(normalizar "$(echo "$linea" | awk '{print $1}')")

    [ -z "$apellido" ] && [ -z "$nombre" ] && continue

    # Usuario = apellido.nombre y password = apellido123
    usuario="$apellido.$nombre"
    contrasena="${apellido}123"
    contenido="${contenido}${usuario}:${contrasena}\n"

    # Si el usuario ya existe no se crea otra vez
    if id "$usuario" >/dev/null 2>&1; then
        echo "El usuario '$usuario' ya existe, se omite."
        continue
    fi

    # Creamos el usuario con su carpeta personal y su shell
    if ! useradd -m -s /bin/bash "$usuario" 2>/dev/null; then
        echo "ERROR: no se pudo crear el usuario '$usuario'."
        continue
    fi

    # Le asignamos la contraseña temporal
    echo "$usuario:$contrasena" | chpasswd

    echo "Cuenta creada: $usuario  (password temporal: $contrasena)"
    total=$((total + 1))
done < <(cat "$LISTA"; echo)

# Actualizamos el archivo con todos los usuarios y contraseñas
printf '%b' "$contenido" > "$ARCHIVO_CUENTAS"
echo "-----------------------------------------"
echo "Archivo con cuentas actualizado: $ARCHIVO_CUENTAS"
echo "Total de cuentas creadas: $total"