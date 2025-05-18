#!/bin/bash
#SBATCH --job-name=POLIANA-THREADS-OMP
#SBATCH --time=0-0:20
#SBATCH --partition=intel-128
#SBATCH --output=slurm-strong-threads-%j.out

# 1. Verifica e carrega módulos disponíveis
echo "Verificando módulos disponíveis..."
module avail gcc
module avail openmpi

# Tenta carregar versões genéricas dos módulos
module load gcc || echo "Módulo gcc não encontrado, usando compilador do sistema"
module load openmpi || echo "Módulo openmpi não encontrado, continuando sem MPI"

# 2. Nome do executável
EXEC="navier_stokes_affinity"

# 3. Compila o código com otimizações (removendo a função não suportada)
echo "Compilando o código..."
gcc -O3 -fopenmp -march=native -o $EXEC navier_stokes_affinity.c -lm

# Verifica se a compilação foi bem sucedida
if [ $? -ne 0 ]; then
    echo "Erro na compilação!"
    exit 1
fi

# 4. Cria um diretório para os resultados
RESULTS_DIR="results_affinity_$(date +%Y%m%d_%H%M%S)"
mkdir -p $RESULTS_DIR

# 5. Nome do arquivo de saída CSV
OUTPUT_CSV="${RESULTS_DIR}/affinity_results.csv"

# 6. Executa o programa e redireciona a saída para o arquivo CSV
echo "Executando o programa..."
echo "affinity_type,num_threads,execution_time" > $OUTPUT_CSV
./$EXEC >> $OUTPUT_CSV

# Verifica se a execução foi bem sucedida
if [ $? -ne 0 ]; then
    echo "Erro na execução do programa!"
    exit 1
fi

echo "Execução concluída com sucesso!"
echo "Resultados salvos em: $OUTPUT_CSV"

# 7. Versão simplificada do código para gerar gráfico (sem Python)
echo "Gerando análise textual dos resultados..."
echo -e "\n=== ANÁLISE DOS RESULTADOS ===" > ${RESULTS_DIR}/analysis.txt
echo "Tempos de execução por configuração:" >> ${RESULTS_DIR}/analysis.txt
cat $OUTPUT_CSV | grep -v "affinity_type" >> ${RESULTS_DIR}/analysis.txt

echo "Análise textual gerada em: ${RESULTS_DIR}/analysis.txt"