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

module load compilers/gnu
module load compilers/nvidia/cuda   # Carrega o ambiente CUDA
module load libraries/cuda          # Carrega as bibliotecas CUDA
module load compilers/nvidia/nvhpc  # Carrega o compilador nvc e ferramentas

nvc -mp=gpu -fast -Minfo=all -o heat heat.c

./heat 1000 10