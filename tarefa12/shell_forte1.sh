#!/bin/bash
#SBATCH --job-name=navier-stokes-forte
#SBATCH --output=slurm-affinity-%j.out
#SBATCH --time=0-0:20
#SBATCH --partition=intel-128

# Compilar o código
echo "[INFO] Compilando main_forte.c..."
gcc -O3 -fopenmp main_forte.c -o main_forte

# Criar arquivo de saída
CSV="resultados_forte.csv"

# Limpar o CSV antes de escrever
> $CSV
echo "n_threads,schedule,collapse,N,tempo" > $CSV

# Parâmetro do problema (fixo para escalabilidade forte)
N=128

# Testar escalabilidade forte com diferentes parâmetros
for threads in 1 2 4 8; do
    for collapse in 1 2 3; do
        for schedule in static dynamic guided; do
            echo "[INFO] Executando com $threads threads | schedule=$schedule | collapse=$collapse"
            OMP_NUM_THREADS=$threads ./main_forte $schedule $collapse $N >> $CSV
        done
    done
done
