#!/bin/bash
#SBATCH --job-name=OMP_heat
#SBATCH --output=%j.out
#SBATCH --error=erro%j.err
#SBATCH --partition=gpu-4-a100
#SBATCH --gpus-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --ntasks-per-node=1
#SBATCH --time=2
#SBATCH --qos=preempt

# Carregar módulos necessários
module load compilers/gnu
module load compilers/nvidia/cuda
module load libraries/cuda
module load compilers/nvidia/nvhpc

echo "Iniciando compilação e execução dos códigos HEAT OpenMP com GPU..."
echo "---------------------------------------------------------------"

# Definição dos parâmetros para execução
N_SIZE=8000   # Tamanho da grade (NxN)
N_STEPS=10    # Número de passos de tempo

# --- Compilação e Execução de heat.c (versão original) ---
echo "Compilando heat.c (versão original)..."
gcc heat.c -o heat_original -fopenmp -lm

if [ -f heat_original ]; then
    echo "Executando heat_original com N=${N_SIZE}, NSTEPS=${N_STEPS}..."
    ./heat_original ${N_SIZE} ${N_STEPS}
else
    echo "Erro na compilação de heat.c"
fi
echo "---------------------------------------------------------------"

# --- Compilação e Execução de heat_target_map.c (com target data e map) ---
echo "Compilando heat_target_map.c (com target data e map)..."
# Usamos nvc para compilar, garantindo suporte completo a OpenMP Offloading para NVIDIA GPUs
nvc heat_target_map.c -o heat_target_map -mp=gpu -gpu=cc80 -lm

if [ -f heat_target_map ]; then
    echo "Executando heat_target_map com N=${N_SIZE}, NSTEPS=${N_STEPS}..."
    ./heat_target_map ${N_SIZE} ${N_STEPS}
else
    echo "Erro na compilação de heat_target_map.c"
fi
echo "---------------------------------------------------------------"

# --- Compilação e Execução de heat_target_map_opt.c (com target data, map e otimização de laço) ---
echo "Compilando heat_target_map_opt.c (com target data, map e otimização de laço)..."
nvc heat_target_map_opt.c -o heat_target_map_opt -mp=gpu -gpu=cc80 -lm

if [ -f heat_target_map_opt ]; then
    echo "Executando heat_target_map_opt com N=${N_SIZE}, NSTEPS=${N_STEPS}..."
    ./heat_target_map_opt ${N_SIZE} ${N_STEPS}
else
    echo "Erro na compilação de heat_target_map_opt.c"
fi
echo "---------------------------------------------------------------"

echo "Processo concluído."