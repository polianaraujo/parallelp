#!/bin/bash
#SBATCH --job-name=OMP_heat_poliana
#SBATCH --output=%j.out
#SBATCH --error=erro%j.err
#SBATCH --partition=gpu-4-a100
#SBATCH --gpus-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --ntasks-per-node=1
#SBATCH --time=2
#SBATCH --qos=preempt

# module load compilers/gnu
# module load compilers/nvidia/cuda
# module load libraries/cuda
# module load compilers/nvidia/nvhpc

# Compilação com suporte OpenMP target
nvc -mp=gpu -fast -o heat heat.c

# Execução
./heat 1000 10
