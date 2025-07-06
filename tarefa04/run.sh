#!/bin/bash

echo "Compilando os programas..."
gcc -o memory memory.c -fopenmp
gcc -o compute compute.c -fopenmp -lm

# Limpa os arquivos de resultados anteriores para uma nova coleta
echo "Limpando arquivos de resultados antigos..."
rm -f memory_results.csv compute_results.csv

# Define a lista de threads para testar (ajuste conforme o seu hardware)
# Ex: para um processador de 8 núcleos e 16 threads
THREAD_LIST="1 2 4 6 8 12 16"

echo "Iniciando a coleta de dados de desempenho..."

for threads in $THREAD_LIST; do
  export OMP_NUM_THREADS=$threads
  echo "--- Executando com $threads thread(s) ---"
  ./memory
  ./compute
done

echo "Coleta de dados finalizada."
echo "Arquivos 'memory_results.csv' e 'compute_results.csv' foram gerados."