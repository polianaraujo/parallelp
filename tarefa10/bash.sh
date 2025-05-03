#!/bin/bash

# Ativando mensagens de compilação
echo "Compilando versão com atomic..."
gcc -fopenmp pi_atomic.c -o pi_atomic -O2

echo "Compilando versão com reduction..."
gcc -fopenmp pi_reduction.c -o pi_reduction -O2

# Executando os programas
echo -e "\n--- Executando pi_atomic ---"
./pi_atomic

echo -e "\n--- Executando pi_reduction ---"
./pi_reduction
