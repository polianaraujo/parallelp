#!/bin/bash
#SBATCH --job-name=POLIANA-THREADS-OMP
#SBATCH --time=0-0:20
#SBATCH --partition=intel-128
#SBATCH --output=slurm-strong-threads-%j.out

# Compilação
echo "Compilando o código C..."
gcc -O3 -fopenmp main.c -o main
if [ $? -ne 0 ]; then
    echo "Erro na compilação."
    exit 1
fi

# Parâmetros de teste
THREADS=(1 2 4 8 16)
AFFINITIES=("false" "true" "close" "spread" "master")

# Cabeçalho do CSV
CSV="results_escalabilidade_forte.csv"
echo "Threads,NX,NY,Schedule,ChunkSize,Affinity,Time,CenterValue,AverageValue" > "$CSV"

# Executar testes
for affinity in "${AFFINITIES[@]}"; do
    export OMP_PROC_BIND=$affinity
    echo "Afinidade: $affinity"

    for t in "${THREADS[@]}"; do
        export OMP_NUM_THREADS=$t
        echo " - Executando com $t threads"

        ./main "$affinity" >> "$CSV"
    done
done

echo "Experimentos concluídos. Resultados salvos em $CSV"
