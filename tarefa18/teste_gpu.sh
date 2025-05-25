#!/bin/bash
#SBATCH --job-name=test_gpu
#SBATCH --output=slurm-test-gpu.out
#SBATCH --error=slurm-test-gpu.err
#SBATCH --partition=gpu-8-v100
#SBATCH --gpus-per-node=1
#SBATCH --time=0-00:05

echo "Testando GPU com nvidia-smi"
nvidia-smi
