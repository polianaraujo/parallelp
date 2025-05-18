#!/bin/bash
#SBATCH --job-name=navier-stokes-fraca
#SBATCH --output=slurm-fraca-%j.out
#SBATCH --time=0-0:20
#SBATCH --partition=intel-128

# Compilar o código
echo "[INFO] Compilando main_fraca.c..."
gcc -fopenmp -O3 main_fraca.c -o main_fraca
if [ $? -ne 0 ]; then
    echo "[ERRO] Falha na compilação"
    exit 1
fi

# Arquivo CSV de saída
CSV="resultados_fraca_.csv"
echo "tipo_execucao,threads,schedule,collapse,N,tempo" > $CSV

# Parâmetros para o teste de escalabilidade fraca
N_BASE=50
COLLAPSES=(1 2)
SCHEDULES=(static dynamic guided)
THREADS=(1 2 4 8 16 32)

for nt in "${THREADS[@]}"; do
    for sched in "${SCHEDULES[@]}"; do
        for c in "${COLLAPSES[@]}"; do
            export OMP_NUM_THREADS=$nt
            echo "[INFO] Executando com $OMP_NUM_THREADS threads | schedule=$sched | collapse=$c | N_base=$N_BASE"
            ./main_fraca $sched $c $N_BASE >> $CSV
        done
    done
done
