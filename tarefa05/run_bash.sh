#!/bin/bash

# Compilar o código com suporte OpenMP
echo "Compilando o programa..."
gcc -fopenmp code.c -o primos -lm

if [ $? -ne 0 ]; then
    echo "Erro na compilação."
    exit 1
fi

# Definir valores de N e números de threads
VALORES_N=(1 10 100 1000 10000 100000 1000000)
THREADS=(1 2 4 8)

# Arquivo CSV de saída
echo "Tipo,N,Threads,TotalPrimos,Tempo" > resultados.csv

# Teste sequencial
for N in "${VALORES_N[@]}"; do
    RESULT=$(echo "$N" | ./primos | grep Sequencial)
    TOTAL=$(echo "$RESULT" | grep "Total de primos" | awk '{print $9}')
    TEMPO=$(echo "$RESULT" | grep "Tempo" | awk '{print $3}')
    echo "Sequencial,$N,1,$TOTAL,$TEMPO" >> resultados.csv
done

# Teste paralelo
for N in "${VALORES_N[@]}"; do
    for T in "${THREADS[@]}"; do
        export OMP_NUM_THREADS=$T
        RESULT=$(echo "$N" | ./primos | grep Paralelo)
        TOTAL=$(echo "$RESULT" | grep "Total de primos" | awk '{print $9}')
        TEMPO=$(echo "$RESULT" | grep "Tempo" | awk '{print $3}')

        THREADS_USADAS=$(echo "$RESULT" | grep "Threads" | awk '{print $3}')
        echo "Paralelo,$N,$THREADS_USADAS,$TOTAL,$TEMPO" >> resultados.csv
    done
done

echo "Testes finalizados. Resultados salvos em resultados.csv."
