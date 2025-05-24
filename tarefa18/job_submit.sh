#!/bin/bash
#SBATCH --job-name=heat
#SBATCH --output=slurm-heat.out
#SBATCH --error=slurm-heat.err
#SBATCH --gpus-per-node=1
#SBATCH --partition=gpu-4-a100
#SBATCH --cpus-per-task=6
#SBATCH --time=0-03:00

# module load nvhpc

gcc -fopenmp -O3 heat.c -o heat

./heat 1000 10