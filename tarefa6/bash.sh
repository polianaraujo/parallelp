#!/bin/bash

# Nome do arquivo CSV de saída
output_file="resultados_pi.csv"

# Cabeçalho do CSV
echo "Metodo,Estimativa_PI" > $output_file

# Função para compilar e executar um programa
run_program() {
    local source_file=$1
    local method_name=$2
    local output
    
    # Compila o programa
    gcc -o temp_program $source_file -fopenmp
    
    # Executa e captura a saída
    output=$(./temp_program)
    
    # Extrai o valor de PI da saída
    pi_value=$(echo "$output" | grep -oE "[0-9]+\.[0-9]+")
    
    # Adiciona ao CSV
    echo "$method_name,$pi_value" >> $output_file
    
    # Remove o programa temporário
    rm -f temp_program
}

# Executa os três programas
run_program "pi_serial.c" "Serial"
run_program "pi_incorreta.c" "Paralelo_Incorreto"
run_program "pi_correta.c" "Paralelo_Correto"

echo "Resultados salvos em $output_file"