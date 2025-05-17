#!/bin/bash
#SBATCH --job-name=POLIANA-FORTE-OMP
#SBATCH --time=0-0:20
#SBATCH --partition=intel-128
#SBATCH --output=slurm-strong-scalability-%j.out

echo "Inicializando JOB: Escalabilidade Forte"

# Compilação
gcc -fopenmp ns_esc_forte.c -o escalabilidade_forte

# Execuções variando threads
# for threads in 1 2 4 8 16 24
# do
export OMP_NUM_THREADS=$threads
echo "Executando com $OMP_NUM_THREADS threads"
/home/pedarajo/repos/parallelp/tarefa12/npad/escalabilidade_forte
# done

echo "Finalizando JOB: Escalabilidade Forte"
