#!/bin/bash

# Ativando mensagens de compilação
echo "Compilando versão com atomic..."
gcc -fopenmp code1.c -o code1 -O2

echo "Compilando versão com reduction..."
gcc -fopenmp code2.c -o code2 -O2

# Executando os programas
echo -e "\n--- Executando pi_atomic ---"
./code1

echo -e "\n--- Executando pi_reduction ---"
./code2
