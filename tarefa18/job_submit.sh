#!/bin/bash
#SBATCH --job-name=heat_poliana
#SBATCH --output=slurm-heat.out
#SBATCH --error=slurm-heat.err
#SBATCH --partition=gpu-8-v100 
#SBATCH --gpus-per-node=1   # Número GPUs por nó
#SBATCH --time=0-03:00

# Compilar com suporte a OpenMP target offload para NVPTX (NVIDIA GPU)
gcc -fopenmp -foffload=nvptx-none -O3 -o heat heat.c -lm

# Executar o programa (ajuste o tamanho e número de passos conforme o experimento)
./heat 1000 500