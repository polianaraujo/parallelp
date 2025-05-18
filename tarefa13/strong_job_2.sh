#!/bin/bash
#SBATCH --job-name=navier-stokes-affinity
#SBATCH --output=slurm-affinity-%j.out
#SBATCH --time=0-0:20
#SBATCH --partition=intel-128
#SBATCH --exclusive

EXEC=navier_stokes_affinity
SRC=main2.c
RESULTS="results_$(date +%Y%m%d_%H%M%S).csv"

echo "threads,affinity,time" > $RESULTS

gcc -O3 -fopenmp -march=native -o $EXEC $SRC -lm || { echo "Erro na compilação"; exit 1; }

for threads in 1 2 4 8 16 32
do
  for affinity in false true close spread master
  do
    export OMP_NUM_THREADS=$threads
    export OMP_PROC_BIND=$affinity
    export OMP_PLACES=cores

    echo "[INFO] Threads=$threads Affinity=$affinity"
    OUTPUT=$(./$EXEC)
    TEMPO=$(echo "$OUTPUT" | grep "Tempo" | awk '{print $2}')
    echo "$threads,$affinity,$TEMPO" >> $RESULTS
  done
done

echo "[INFO] Resultados em $RESULTS"
