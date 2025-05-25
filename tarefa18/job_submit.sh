#!/bin/bash
#SBATCH --job-name=heat_poliana
#SBATCH --output=slurm-heat.out
#SBATCH --error=slurm-heat.err
#SBATCH --partition=gpu-8-v100 
#SBATCH --gpus-per-node=1
#SBATCH --time=0-00:10

set -x

echo "Iniciando job SLURM..."

module purge
module load gcc/12.2.0
module load cuda/11.8

echo "Compilando o código..."
gcc -fopenmp -foffload=nvptx-none -O3 -o heat heat.c -lm

echo "Listando arquivos após compilação:"
ls -lh

if [ ! -f heat ]; then
    echo "Erro: arquivo executável 'heat' não foi criado"
    exit 1
fi

echo "Executando o programa..."
./heat 1000 500
