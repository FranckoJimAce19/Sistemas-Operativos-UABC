#!/bin/bash
n=20

# Si reciben un parametro lo tomamos como cantidad (maximo 20)
if [ $# -gt 0 ]; then
    n=$1
    if [ "$n" -gt 20 ]; then
        echo "El maximo permitido es 20, se usara 20."
        n=20
    fi
fi

# Arreglo que inicia con los dos primeros numeros
fib[0]=0
fib[1]=1

# Calculamos cada numero como la suma de los dos anteriores
for (( i = 2; i < n; i++ )); do
    fib[$i]=$(( fib[$((i - 1))] + fib[$((i - 2))] ))
done

# Imprimimos
echo "${fib[*]:0:$n}" | tr ' ' ','