#!/bin/bash
#SBATCH --job-name=POLIANA-FRACA-OMP
#SBATCH --time=0-0:20
#SBATCH --partition=intel-128
#SBATCH --output=slurm-weak-scalability-%j.out

echo "Inicializando JOB: Escalabilidade Fraca"

# Compilação
echo "Compilando ns_esc_fraca.c..."
gcc -fopenmp ns_esc_fraca.c -o escalabilidade_fraca
echo "Compilação concluída."

# Listar conteúdo do diretório atual
echo "Arquivos no diretório:"
ls -lh

# Verificar se o executável existe
if [ ! -f escalabilidade_fraca ]; then
    echo "Erro: executável não encontrado."
    exit 1
fi

# Executar o programa
echo "Executando ./escalabilidade_fraca"
/home/pedarajo/repos/parallelp/tarefa12/npad/escalabilidade_fraca

echo "Finalizando JOB: Escalabilidade Fraca"
