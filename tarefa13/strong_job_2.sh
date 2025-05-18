#!/bin/bash
#SBATCH --job-name=poliana-omp-test
#SBATCH --time=00:20:00
#SBATCH --partition=intel-128
#SBATCH --output=slurm-out-%j.txt

echo "Iniciando job SLURM..."

# 1. Compila o código
EXEC="navier_stokes"
SRC="main.c"

echo "Compilando com GCC e OpenMP..."
gcc -O3 -fopenmp -march=native -o $EXEC $SRC -lm
if [ $? -ne 0 ]; then
    echo "Erro na compilação."
    exit 1
fi

# 2. Diretório de resultados
DIR="results_$(date +%Y%m%d_%H%M%S)"
mkdir -p "$DIR"
OUTPUT="$DIR/result.csv"

# 3. Executa e salva resultados
echo "Executando simulação..."
echo "num_threads,execution_time" > "$OUTPUT"
./$EXEC >> "$OUTPUT"

# 4. Análise textual simples
echo "Gerando análise..."
echo -e "\n=== TEMPOS DE EXECUÇÃO POR THREADS ===" > "$DIR/analysis.txt"
cat "$OUTPUT" | grep -v num_threads >> "$DIR/analysis.txt"

echo "Resultados salvos em: $OUTPUT"
