#!/bin/bash
#SBATCH --job-name=heat-mpi
#SBATCH --output=slurm-heat-mpi-%j.out
#SBATCH --time=0-0:20
#SBATCH --partition=intel-128
#SBATCH --ntasks=4
#SBATCH --nodes=1
#SBATCH --exclusive

# Compilação (opcional, remova se você já compilou)
mpicc -O2 heat_mpi_sendrecv.c -o heat_sendrecv
mpicc -O2 heat_mpi_isend_irecv.c -o heat_isend_irecv
mpicc -O2 heat_mpi_test.c -o heat_test

# Execução das três versões
echo "== Iniciando versão com MPI_Send/MPI_Recv =="
mpirun -np 4 ./heat_sendrecv

echo "== Iniciando versão com MPI_Isend/Irecv + Wait =="
mpirun -np 4 ./heat_isend_irecv

echo "== Iniciando versão com MPI_Test para sobreposição =="
mpirun -np 4 ./heat_test
