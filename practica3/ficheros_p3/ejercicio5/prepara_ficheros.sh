#!/bin/bash

# Comprobar que se ha pasado un argumento
if [ "$#" -ne 1 ]; then
    echo "Uso: $0 <directorio>"
    exit 1
fi

DIRECTORIO="$1"

# Crear el directorio si no existe y limpiar su contenido si ya existe
if [ -d "$DIRECTORIO" ]; then
    rm -rf "$DIRECTORIO/*"
else
    mkdir -p "$DIRECTORIO"
fi

# Cambiar al directorio especificado
cd "$DIRECTORIO"

# Crear los ficheros y el directorio
mkdir subdir
touch fichero1
echo "Contenido" > fichero2
ln -s fichero2 enlaceS
ln fichero2 enlaceH

# Mostrar atributos de los ficheros
stat fichero1
stat fichero2
stat subdir
stat enlaceS
stat enlaceH
