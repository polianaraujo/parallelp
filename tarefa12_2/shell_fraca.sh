#!/bin/bash
#SBATCH --job-name=navier-stokes-fraca
#SBATCH --output=slurm-fraca-%j.out
#SBATCH --time=0-0:20
#SBATCH --partition=intel-128

#!/bin/bash

# Compilar o programa (se ainda não estiver compilado)
gcc -fopenmp main_fraca.c -o main_fraca

# Parâmetros fixos
N_BASE=50  # Tamanho base do problema por thread
OUTPUT="resultados_fraca_.csv"

# Cabeçalho do CSV
echo "threads,schedule,collapse,N,tempo" > $OUTPUT

# Loop de execução
for threads in 1 2 4 8; do
    export OMP_NUM_THREADS=$threads
    for schedule in static dynamic guided; do
        for collapse in 1 2 3; do
            ./main_fraca $schedule $collapse $N_BASE >> $OUTPUT
        done
    done
done
