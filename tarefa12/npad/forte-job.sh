#!/bin/bash
#SBATCH --job-name=POLIANA-STRONG-OMP
#SBATCH --time=0-0:20
#SBATCH --partition=intel-512
#SBATCH --output=slurm-strong-scalability-%j.out

echo "Inicializando JOB: Escalabilidade Forte"

# Compilação
gcc -fopenmp escalabilidade_forte.c -o strong-scalability-assessment

# Execuções variando threads
for threads in 1 2 4 8 16 24
do
  export OMP_NUM_THREADS=$threads
  echo "Executando com $OMP_NUM_THREADS threads"
  ./strong-scalability-assessment
done

echo "Finalizando JOB: Escalabilidade Forte"
