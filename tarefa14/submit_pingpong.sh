#!/bin/bash
#SBATCH --job-name=pingpong-mpi
#SBATCH --output=slurm-pingpong-%j.out
#SBATCH --time=0-0:20
#SBATCH --partition=intel-128
#SBATCH --exclusive
#SBATCH --nodes=1
#SBATCH --ntasks=2

# Carrega o módulo MPI, se necessário (ajuste conforme o ambiente do NPAD)
module load mpi/openmpi-x86_64

# Compila o código (se ainda não estiver compilado)
mpicc -o pingpong pingpong.c

# Executa o programa com 2 processos MPI
mpirun -np 2 ./pingpong
