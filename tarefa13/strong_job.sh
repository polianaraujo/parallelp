#!/bin/bash
#SBATCH --job-name=POLIANA-THREADS-OMP
#SBATCH --time=0-0:20
#SBATCH --partition=intel-128
#SBATCH --output=slurm-strong-threads-%j.out

# Compilar o código
echo "Compilando o código C..."
gcc -fopenmp -O2 -o main main.c
if [ $? -ne 0 ]; then
    echo "Erro na compilação."
    exit 1
fi
echo "Compilação concluída."

# Valores de affinities (OpenMP usa OMP_PROC_BIND para controle de afinidade)
AFFINITIES=("false" "true" "close" "spread" "master")

# Executar o programa com diferentes configurações de OMP_PROC_BIND
for AFFINITY in "${AFFINITIES[@]}"
do
    echo "Executando com OMP_PROC_BIND=$AFFINITY"
    export OMP_PROC_BIND=$AFFINITY
    ./main "$AFFINITY"
done