#!/bin/bash
#SBATCH --job-name=POLIANA-FRACA-OMP
#SBATCH --time=0-0:20
#SBATCH --partition=intel-128
#SBATCH --output=slurm-weak-scalability-%j.out

echo "Inicializando JOB: Escalabilidade Fraca"

# Compilação
gcc -fopenmp ns_esc_fraca.c -o escalabilidade_fraca

# Execuções variando threads
# for threads in 1 2 4 8 16 24
# do
# export OMP_NUM_THREADS=$threads
echo "Executando escalabilidade fraca com variação de threads e tamanho do problema"
# /home/pedarajo/repos/parallelp/tarefa12/npad/escalabilidade_fraca
./escalabilidade_fraca
# done

echo "Finalizando JOB: Escalabilidade Fraca"
