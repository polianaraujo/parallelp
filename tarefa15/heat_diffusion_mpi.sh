#!/bin/bash
#SBATCH --job-name=heat-mpi
#SBATCH --output=slurm-heat-mpi-%j.out
#SBATCH --time=0-0:20
#SBATCH --partition=intel-128
#SBATCH --ntasks=4
#SBATCH --nodes=1
#SBATCH --exclusive

mpicc -O2 heat_diffusion_sendrecv.c -o heat_sendrecv
mpicc -O2 heat_diffusion_isend_wait.c -o heat_isend_irecv
mpicc -O2 heat_diffusion_overlap.c -o heat_test

echo "== Iniciando versão com MPI_Send/MPI_Recv =="
mpirun -np 4 ./heat_sendrecv

echo "== Iniciando versão com MPI_Isend/Irecv + Wait =="
mpirun -np 4 ./heat_isend_irecv

echo "== Iniciando versão com MPI_Test para sobreposição =="
mpirun -np 4 ./heat_test
