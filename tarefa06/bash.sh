#!/bin/bash

# Criar o arquivo CSV e adicionar o cabeçalho
echo "Versao,Pi,Tempo(s)" > resultados.csv

# Compilar e rodar a versão Serial
gcc -fopenmp -o serial mc_serial.c
serial_output=$(./serial)
pi_serial=$(echo "$serial_output" | grep "Estimativa" | awk '{print $4}')
tempo_serial=$(echo "$serial_output" | grep "Tempo" | awk '{print $2}')
echo "Serial,$pi_serial,$tempo_serial" >> resultados.csv

# Compilar e rodar a versão Paralela Incorreta
gcc -fopenmp -o incorreta mc_incorreta.c
incorreta_output=$(./incorreta)
pi_incorreta=$(echo "$incorreta_output" | grep "Estimativa" | awk '{print $4}')
tempo_incorreta=$(echo "$incorreta_output" | grep "Tempo" | awk '{print $2}')
echo "Paralela Incoreta,$pi_incorreta,$tempo_incorreta" >> resultados.csv

# Lista de versões corretas com diferentes cláusulas
VERSOES=("default" "private" "shared" "firstprivate" "lastprivate")

# Compilar e rodar cada uma
for versao in "${VERSOES[@]}"; do
    gcc -fopenmp -o correta_$versao mc_correta_$versao.c
    saida=$(./correta_$versao)
    pi=$(echo "$saida" | grep "Estimativa" | awk '{print $4}')
    tempo=$(echo "$saida" | grep "Tempo" | awk '{print $2}')
    echo "Paralela Corret-$versao,$pi,$tempo" >> resultados.csv
done

echo "Resultados salvos em resultados.csv"
