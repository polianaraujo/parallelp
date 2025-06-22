#!/bin/bash

# Nome do arquivo C, do executável e do arquivo de resultados
C_FILE="codigo.c"
EXEC_FILE="ilp_test"
CSV_FILE="results.csv"

# Níveis de otimização a serem testados
OPTIMIZATION_LEVELS="O0 O2 O3"

# 1. Cria o arquivo CSV e escreve o cabeçalho
echo "Optimization,Function,ExecutionTime_s" > $CSV_FILE

# 2. Loop através dos níveis de otimização
for opt in $OPTIMIZATION_LEVELS; do
    echo "--- Compilando com -$opt ---"
    # Compila o código com o nível de otimização atual
    gcc -$opt -o $EXEC_FILE $C_FILE

    if [ $? -eq 0 ]; then
        echo "--- Executando para -$opt ---"
        # Executa o programa, passando o nome da otimização como argumento
        ./$EXEC_FILE $opt
    else
        echo "Falha na compilação com -$opt"
    fi
    echo ""
done

# 3. Limpa o arquivo executável
rm $EXEC_FILE

echo "Experimento concluído. Resultados salvos em $CSV_FILE"