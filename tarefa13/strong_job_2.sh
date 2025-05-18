#!/bin/bash
#SBATCH --job-name=poliana-omp-test
#SBATCH --time=00:20:00
#SBATCH --partition=intel-128
#SBATCH --output=slurm-out-%j.txt

# Diretório de trabalho
EXEC=navier_stokes_affinity
SRC=main2.c
RESULTS_DIR="results_affinity_$(date +%Y%m%d_%H%M%S)"
mkdir -p $RESULTS_DIR

echo "[INFO] Compilando código com OpenMP..."
gcc -O3 -fopenmp -march=native -o $EXEC $SRC -lm || { echo "Erro na compilação"; exit 1; }

echo "[INFO] Executando simulação..."
./$EXEC > $RESULTS_DIR/results.csv

echo "[INFO] Resultados salvos em $RESULTS_DIR/results.csv"

echo "[INFO] Fim da execução."