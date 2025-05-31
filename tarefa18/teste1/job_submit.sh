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
module load compilers/nvidia/cuda   # ESSENCIAL
module load libraries/cuda          # ESSENCIAL
module load compilers/nvidia/nvhpc  # ESSENCIAL para o nvc e ferramentas

# Compilação (certifique-se que o nome do arquivo-fonte está correto: heat.c)
nvc -mp=gpu -fast -Minfo=all -o heat heat.c

# Execução
./heat 1000 10