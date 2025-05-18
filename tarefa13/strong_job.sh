#!/bin/bash
#SBATCH --job-name=POLIANA-THREADS-OMP
#SBATCH --time=0-0:20
#SBATCH --partition=intel-128
#SBATCH --output=slurm-strong-threads-%j.out

# Apaga o arquivo de resultados antigo para evitar mistura de dados
rm -f results_escalabilidade_forte.csv

# Define parâmetros fixos da simulação
NX=100
NY=100
SCHEDULE="static"
CHUNK=4

# Define as afinidades a testar
AFFINITIES=("false" "true" "close" "spread" "master")

# Define os números de threads que vai testar
THREADS=(1 2 4 8 16)

# Loop para testar todas as combinações
for affinity in "${AFFINITIES[@]}"; do
    echo "Testando afinidade: $affinity"
    
    # Configura variáveis de ambiente OpenMP conforme a afinidade
    case $affinity in
        false)
            unset OMP_PLACES
            unset OMP_PROC_BIND
            ;;
        true)
            export OMP_PLACES=cores
            export OMP_PROC_BIND=true
            ;;
        close)
            export OMP_PLACES=cores
            export OMP_PROC_BIND=close
            ;;
        spread)
            export OMP_PLACES=cores
            export OMP_PROC_BIND=spread
            ;;
        master)
            export OMP_PLACES=cores
            export OMP_PROC_BIND=master
            ;;
        *)
            unset OMP_PLACES
            unset OMP_PROC_BIND
            ;;
    esac

    for nt in "${THREADS[@]}"; do
        echo "  Rodando com $nt threads..."
        export OMP_NUM_THREADS=$nt
        ./seu_programa "$affinity"
    done
done

echo "Todos os testes finalizados."