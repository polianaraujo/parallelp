#!/bin/bash
#SBATCH --job-name=heat-gpu-poliana
#SBATCH --output=slurm-heat.out
#SBATCH --error=slurm-heat.err
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --gpus=1
#SBATCH --time=00:10:00
#SBATCH --partition=gpu-8-v100

# Carrega os módulos necessários
# module load gcc/12.2.0
# module load openmpi/4.1.4
# module load cuda/12.2.0

# Compilação com suporte a OpenMP offloading para GPU NVIDIA
gcc -fopenmp -foffload=nvptx-none -O3 heat.c -o heat

# Executa o programa com grade 1000x1000 e 100 passos de tempo
./heat 1000 100
