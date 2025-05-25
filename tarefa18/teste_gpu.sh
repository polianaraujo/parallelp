#!/bin/bash
#SBATCH --job-name=test_simple
#SBATCH --output=teste_simple.out
#SBATCH --error=teste_simple.err
#SBATCH --time=0-03:00

echo "Testando SLURM no NPAD - GPU partition"
nvidia-smi

