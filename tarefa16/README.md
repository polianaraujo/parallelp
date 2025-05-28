# Relatório 16 - Comunicação Coletiva com MPI
Aluna: Poliana Ellen de Araújo

## 1. Introdução

Este repositório contém um programa MPI em C para calcular o produto y=A⋅x, onde A é uma matriz $M×N$ e $x$ é um vetor de tamanho $N$. O objetivo é demonstrar e analisar o desempenho da paralelização usando MPI em diferentes cenários de tamanho de matriz e número de processos.

A multiplicação de matrizes por vetores é uma operação fundamental em diversas áreas da computação científica e engenharia. Para grandes dimensões, a execução sequencial pode ser demorada. Este projeto explora como a paralelização usando o padrão de Message Passing Interface (MPI) pode acelerar essa operação, distribuindo a carga de trabalho entre múltiplos processos.

### Conteúdo

- ``matrix_vector_mpi.c``: O código-fonte em C da implementação MPI.
- ``run_experiments.sh``: Um script Bash para compilar e executar o programa com várias configurações de matriz e número de processos.
- ``resultados.csv``: Arquivo CSV que armazena os tempos de execução coletados pelos experimentos.
- ``README.md``: Este arquivo.

## 2. Metodologia

O programa implementa a multiplicação y=A⋅x da seguinte forma:

1. Divisão da Matriz A: A matriz A é dividida por linhas entre os processos usando MPI_Scatter. Cada processo recebe um subconjunto de linhas da matriz.
2. Distribuição do Vetor x: O vetor x completo é broadcast para todos os processos usando MPI_Bcast, garantindo que cada processo tenha acesso a todos os elementos de x.
3. Cálculo Local: Cada processo calcula os elementos de y correspondentes às suas linhas da matriz A e ao vetor x.
4. Agregação dos Resultados: Os resultados parciais (local_y) de cada processo são coletados de volta ao processo raiz usando MPI_Gather, que os combina para formar o vetor y final.
5. Medição de Tempo: O tempo de execução é medido do início da distribuição dos dados até o final da coleta dos resultados.

O script run_experiments.sh automatiza a execução do programa para os seguintes parâmetros:
- Tamanhos da Matriz M: 1000, 10000, 50000
- Tamanhos do Vetor N: 100, 1000
- Número de Processos (NP): 2, 4, 8
É feita uma verificação para garantir que M seja divisível por NP para uma distribuição equitativa.

### Compilação

```
sbatch job_submit.sh
```

## 3. Resultados

![Comparação das afinidades de todas as threads](https://github.com/polianaraujo/parallelp/blob/main/tarefa16/tempo_vs_processos_tamanhos_matrizes.png)

## 4. Conclusões

- 📈 Tendências observadas
    - ✅ Eficiência do paralelismo:
        - A paralelização melhora o desempenho até certo ponto.
        - O número ótimo de processos varia com o tamanho do problema (M e N).

- ❌ Overhead de comunicação:
    - Quando M ou N são pequenos, muitos processos introduzem mais overhead do que benefício.
    - Quando M é grande o suficiente, o paralelismo compensa o overhead — mas até um certo ponto.


- Problemas pequenos (ex: M=1000, N=100): poucos processos já são suficientes; muitos processos não trazem ganho significativo.

- Problemas médios (ex: M=10000, N=1000): 4 a 8 processos são eficazes, mas deve-se monitorar a sobrecarga.

- Problemas grandes (ex: M=50000, N=1000): 4 processos parecem ser o ponto de equilíbrio; 8 processos aumentam a comunicação e não compensam em desempenho.