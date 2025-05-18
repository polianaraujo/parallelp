#!/bin/bash
#SBATCH --job-name=POLIANA-THREADS-OMP
#SBATCH --time=0-0:20
#SBATCH --partition=intel-128
#SBATCH --output=slurm-strong-threads-%j.out

# Remover o executável anterior e diretório de resultados
rm -f main
rm -rf results_escalabilidade_forte.csv

echo "Compilando o código C..."
gcc -O3 -fopenmp -o main main.c
if [ $? -ne 0 ]; then
  echo "Erro na compilação!"
  exit 1
fi
echo "Compilação concluída."

AFFINITIES=("false" "true" "close" "spread" "master")

for AFFINITY in "${AFFINITIES[@]}"
do
  echo "Executando com OMP_PROC_BIND=$AFFINITY"
  export OMP_PROC_BIND=$AFFINITY
  ./main "$AFFINITY"
done
