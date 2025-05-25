#!/bin/bash
#SBATCH --job-name=heat_poliana_cpu
#SBATCH --output=slurm-heat.out
#SBATCH --error=slurm-heat.err
#SBATCH --partition=cpu-small
#SBATCH --time=0-00:10
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4

set -x

echo "Iniciando job SLURM (CPU)..."

module purge
module load gcc/12.2.0

echo "Compilando o código..."
gcc -fopenmp -O3 -o heat heat.c -lm

echo "Listando arquivos após compilação:"
ls -lh

if [ ! -f heat ]; then
    echo "Erro: arquivo executável 'heat' não foi criado"
    exit 1
fi

echo "Executando o programa..."
./heat 1000 500
