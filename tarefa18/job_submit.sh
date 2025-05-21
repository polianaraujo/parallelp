#!/bin/bash
#SBATCH --job-name=heat
#SBATCH --output=slurm-heat.out
#SBATCH --error=slurm-heat.err
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=8
#SBATCH --gres=gpu-8-v100
#SBATCH --time=00:10:00

# module load nvhpc

nvc -mp=gpu -O3 heat.c -o heat

./heat 1000 10

