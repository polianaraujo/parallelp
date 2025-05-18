#!/bin/bash
#SBATCH --job-name=POLIANA-THREADS-OMP
#SBATCH --time=0-0:20
#SBATCH --partition=intel-128
#SBATCH --output=slurm-strong-threads-%j.out

# Carrega o módulo do compilador (ajuste conforme necessário)
module load gcc/9.3.0
module load openmpi/4.0.3

# Nome do executável
EXEC="navier_stokes_affinity"

# Compila o código com otimizações
echo "Compilando o código..."
gcc -O3 -fopenmp -march=native -o $EXEC main2.c -lm

# Verifica se a compilação foi bem sucedida
if [ $? -ne 0 ]; then
    echo "Erro na compilação!"
    exit 1
fi

# Cria um diretório para os resultados
RESULTS_DIR="results_affinity_$(date +%Y%m%d_%H%M%S)"
mkdir -p $RESULTS_DIR

# Nome do arquivo de saída CSV
OUTPUT_CSV="${RESULTS_DIR}/affinity_results.csv"

# Executa o programa e redireciona a saída para o arquivo CSV
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

# Opcional: Gera um gráfico com os resultados (requer Python e matplotlib)
echo "Gerando gráfico de resultados..."
module load python/3.8.0
cat << 'EOF' > ${RESULTS_DIR}/plot_results.py
import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('affinity_results.csv')
pivot_df = df.pivot(index='num_threads', columns='affinity_type', values='execution_time')

plt.figure(figsize=(10,6))
for column in pivot_df.columns:
    plt.plot(pivot_df.index, pivot_df[column], marker='o', label=column)

plt.title('Escalabilidade do Código de Navier-Stokes')
plt.xlabel('Número de Threads')
plt.ylabel('Tempo de Execução (s)')
plt.legend(title='Afinidade')
plt.grid(True)
plt.savefig('affinity_scaling.png')
plt.close()
EOF

cd $RESULTS_DIR
python plot_results.py
cd ..

echo "Gráfico gerado em: ${RESULTS_DIR}/affinity_scaling.png"