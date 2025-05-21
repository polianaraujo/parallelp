#!/bin/bash
#SBATCH --job-name=matrix-vector-mpi
#SBATCH --output=slurm-matvec-%j.out
#SBATCH --time=0-0:30
#SBATCH --partition=intel-128
#SBATCH --ntasks=16
#SBATCH --cpus-per-task=1

# module load mpi/openmpi-x86_64

mpicc matrix_vector_mpi.c -o matrix_vector_mpi -O2

# Testes com diferentes M, N e np
for M in 1000 10000 50000; do
  for N in 100 1000; do
    for NP in 2 4 8 16; do
      if (( M % NP == 0 )); then
        echo "=== Executando M=$M N=$N com $NP processos ==="
        mpirun --mca btl ^openib -np $NP ./matrix_vector_mpi $M $N
        echo ""
      else
        echo ">>> Pulando M=$M com $NP processos (não divisível)"
      fi
    done
  done
done
