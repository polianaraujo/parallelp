#!/bin/bash
#SBATCH --job-name=matvec-col
#SBATCH --output=slurm-matvec-col-%j.out
#SBATCH --time=0-0:20
#SBATCH --partition=intel-256
#SBATCH --ntasks=4
#SBATCH --cpus-per-task=1

# module load mpi/openmpi-x86_64

mpicc -o matvec_col matvec_col.c
mpirun -np 4 ./matvec_col
