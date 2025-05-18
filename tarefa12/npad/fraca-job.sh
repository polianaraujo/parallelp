#!/bin/bash
#SBATCH --job-name=POLIANA-FRACA-OMP
#SBATCH --time=0-0:20
#SBATCH --partition=intel-128
#SBATCH --output=slurm-weak-scalability-%j.out

echo "Inicializando JOB: Escalabilidade Fraca"

# Compilação
gcc -fopenmp fraca_stdout.c -o escalabilidade_fraca

# Execução única (o próprio código varia threads e dimensões)
echo "Executando escalabilidade fraca com variação de threads e tamanho do problema"
/home/pedarajo/repos/parallelp/tarefa12/npad/escalabilidade_fraca

echo "Finalizando JOB: Escalabilidade Fraca"
