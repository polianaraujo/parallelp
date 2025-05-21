#!/bin/bash
#SBATCH --job-name=matrix-vector-mpi
#SBATCH --output=slurm-matvec-%j.out
#SBATCH --time=0-01:00  # aumenta tempo de execução
#SBATCH --partition=intel-256
#SBATCH --ntasks=32
#SBATCH --cpus-per-task=1

# module load mpi/openmpi-x86_64

mpicc matrix_vector_mpi.c -o matrix_vector_mpi -O2

# Cabeçalho do CSV
echo "M,N,Num_processos,Tempo_segundos" > resultados.csv

# Conjuntos maiores de M, N e NP
for M in 1000 10000 50000 100000; do
  for N in 100 1000 2000; do
    for NP in 2 4 8 16 32; do
      if (( N % NP == 0 )); then
        echo "=== Executando M=$M N=$N com $NP processos ==="
        mpirun --mca btl ^openib -np $NP ./matrix_vector_mpi $M $N
        echo ""
      else
        echo ">>> Pulando N=$N com $NP processos (não divisível)"
      fi
    done
  done
done
