#!/bin/bash

# Pedimos los numeros al usuario
read -p "Primer numero: " n1
read -p "Segundo numero: " n2

# sumamos, restamos y multiplicamos (resultado con hasta 4 decimales)
echo "Suma:            $(echo "scale=4; $n1 + $n2" | bc)"
echo "Resta:           $(echo "scale=4; $n1 - $n2" | bc)"
echo "Multiplicacion:  $(echo "scale=4; $n1 * $n2" | bc)"

# Si el segundo numero es 0 no se puede dividir
if [ "$(echo "$n2 == 0" | bc)" -eq 1 ]; then
    echo "Division:        No se puede dividir entre cero."
else
    echo "Division:        $(echo "scale=4; $n1 / $n2" | bc)"
fi

# Comparamos cual numero es mayor o si son iguales
if [ "$(echo "$n1 > $n2" | bc)" -eq 1 ]; then
    echo "$n1 es mayor que $n2."
elif [ "$(echo "$n1 < $n2" | bc)" -eq 1 ]; then
    echo "$n1 es menor que $n2."
else
    echo "Ambos numeros son iguales."
fi