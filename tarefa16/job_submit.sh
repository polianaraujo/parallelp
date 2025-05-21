#!/bin/bash
#SBATCH --job-name=matrix-vector-mpi
#SBATCH --output=slurm-matvec-%j.out
#SBATCH --time=0-0:20
#SBATCH --partition=intel-128
#SBATCH --ntasks=4
#SBATCH --cpus-per-task=1

# module load mpi/openmpi-x86_64

mpicc matrix_vector_mpi.c -o matrix_vector_mpi
mpirun -np 4 ./matrix_vector_mpi
