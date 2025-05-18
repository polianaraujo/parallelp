#!/bin/bash
#SBATCH --job-name=navier-stokes-affinity
#SBATCH --output=slurm-affinity-%j.out
#SBATCH --time=0-0:20
#SBATCH --partition=intel-128

# Compila o código, se necessário
make clean && make

# Define arquivo de saída
CSV="resultados_navier_stokes.csv"
echo "tipo_execucao,n_threads,schedule,collapse,N,tempo" > $CSV

# Testes de escalabilidade FORTE: problema fixo (N = 128³)
for threads in 1 2 4 8 16 32 64 128; do
    for collapse in 1 2 3; do
        for sched in static dynamic guided; do
            ./simulador forte $sched $collapse $threads >> $CSV
        done
    done
done

# Testes de escalabilidade FRACA: problema cresce com threads
for threads in 1 2 4 8 16 32 64 128; do
    for collapse in 1 2 3; do
        for sched in static dynamic guided; do
            ./simulador fraca $sched $collapse $threads >> $CSV
        done
    done
done
