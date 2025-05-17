#!/bin/bash
#SBATCH --job-name=POLIANA-WEAK-OMP
#SBATCH --time=0-0:20
#SBATCH --partition=intel-512
#SBATCH --output=slurm-weak-scalability-%j.out

echo "Inicializando JOB: Escalabilidade Fraca"

# Compilação
gcc -fopenmp escalabilidade_fraca.c -o weak-scalability-assessment

baseN=32  # base N para 1 thread

for threads in 1 2 4 8 16 24
do
  export OMP_NUM_THREADS=$threads

  # Calcula N proporcional ao número de threads, mantendo volume constante por thread
  N=$(awk "BEGIN { print int($baseN * ($threads ^ (1.0/3.0))) }")

  if (( N < 3 )); then
    echo "N calculado muito pequeno ($N). Ajustando para N=3"
    N=3
  fi

  echo "Executando com $OMP_NUM_THREADS threads e N=$N"
  ./weak-scalability-assessment $N
done

echo "Finalizando JOB: Escalabilidade Fraca"
