#!/bin/bash

# Compilar o programa
gcc -fopenmp primos.c -o primos

# Arquivo CSV de saída
echo "n,sequencial,paralelo" > resultados.csv

# Valores de n para testar
for n in {1..10000000..1000000}
do
    echo "Rodando para n=$n"
    resultado=$(./primos <<< "$n")

    # Extrair os tempos usando grep e awk
    tempo_seq=$(echo "$resultado" | grep "Tempo sequencial" | awk '{print $3}')
    tempo_par=$(echo "$resultado" | grep "Tempo paralelo" | awk '{print $3}')

    # Adicionar linha no CSV
    echo "$n,$tempo_seq,$tempo_par" >> resultados.csv
done

echo "Benchmark finalizado. Resultados em resultados.csv"
