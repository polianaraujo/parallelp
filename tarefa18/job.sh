#!/bin/bash

# Nome do arquivo fonte C
SOURCE_FILE="heat.c"

# Nome do executável de saída
EXECUTABLE="heat_nvc"

# Argumentos padrão para o programa (tamanho da grade e número de timesteps)
DEFAULT_N=1000
DEFAULT_NSTEPS=100

# Verifica se os argumentos foram passados no script
if [ -n "$1" ]; then
    N=$1
else
    N=$DEFAULT_N
fi

if [ -n "$2" ]; then
    NSTEPS=$2
else
    NSTEPS=$DEFAULT_NSTEPS
fi

echo "--- Compilando $SOURCE_FILE com nvc ---"
# Comando de compilação
# -mp=gpu: habilita o offloading OpenMP para GPU
# -o: define o nome do executável de saída
# -lm: linka a biblioteca matemática
nvc -mp=gpu -o "$EXECUTABLE" "$SOURCE_FILE" -lm

# Verifica se a compilação foi bem-sucedida
if [ $? -eq 0 ]; then
    echo "Compilação bem-sucedida! Executando $EXECUTABLE..."
    echo "Usando N=$N e NSTEPS=$NSTEPS"
    # Comando de execução
    "./$EXECUTABLE" "$N" "$NSTEPS"
else
    echo "Erro na compilação."
fi

echo "--- Fim da execução ---"