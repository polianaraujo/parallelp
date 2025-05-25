#!/bin/bash
#SBATCH --job-name=heat_poliana_gpu
#SBATCH --output=slurm-heat-gpu.out
#SBATCH --error=slurm-heat-gpu.err
#SBATCH --partition=gpu-8-v100
#SBATCH --gpus-per-node=1          # Número de GPUs por nó
#SBATCH --time=0-03:00             # Tempo máximo (3 horas)

echo "Iniciando job SLURM (GPU)..."

# Opcional: verificar GPUs disponíveis com nvidia-smi
nvidia-smi

# Compilar o programa (se precisar de código GPU, ajuste a compilação)
# Aqui, exemplo com nvcc para código CUDA, ou gcc se não for CUDA:
# nvcc -O3 -o heat_cuda heat_cuda.cu -lm
# Ou, se seu programa é CPU, apenas com gcc:
gcc -fopenmp -O3 -o heat heat.c -lm

echo "Executando o programa na GPU..."
./heat 1000 500
