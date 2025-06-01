# Relatório 17 - Estruturas de Dados Não Contíguas em MPI
Aluna: Poliana Ellen de Araújo

## 1. Introdução

Este projeto implementa a multiplicação de uma matriz por um vetor $(y=A⋅x)$ de forma paralela utilizando MPI (Message Passing Interface). A principal característica desta implementação é a distribuição das colunas da matriz A entre os processos, uma estratégia que difere da mais comum distribuição por linhas e explora o uso de tipos de dados derivados MPI para otimizar a comunicação.

## 2. Metodologia

A matriz $A$ de dimensões $M×N$ e o vetor $x$ de dimensão $N$ são divididos entre $P$ processos. Cada processo recebe um bloco de $N/P$ colunas de $A$ e a porção correspondente de $N/P$ elementos de $x$.

1. **Distribuição de Dados**: O processo raiz (rank 0) distribui as colunas da matriz $A$ e os segmentos do vetor $x$ para os outros processos. Para a matriz $A$, são utilizados tipos de dados derivados `MPI` (`MPI_Type_vector` e `MPI_Type_create_resized`) para lidar com o armazenamento em _row-major_ order (linha principal) e garantir uma distribuição eficiente.

2. **Cálculo Parcial Local**: Cada processo calcula uma contribuição parcial para todos os elementos do vetor resultado $y$. Isso significa que, para cada elemento $y_i$, um processo calcula $\sum_{j\in colunas\_ locais}A_{ij} \cdot x_j$.

3. **Agregação de Resultados**: As contribuições parciais de $y$ de todos os processos são somadas elemento a elemento no processo raiz usando a operação de redução `MPI_Reduce` com `MPI_SUM`.

Para executar no NPAD, basta executar:
```
sbatch job_submit.sh
```

## 3. Resultados

Os resultados foram registrados em um arquivo CSV, que foi utilizado para gerar o gráfico para fazer a análise entre o tempo de execução com para tamanho de matriz e o número de processos.

![Tamanho da matriz por número de processos vs Tempo de execução.](https://github.com/polianaraujo/parallelp/blob/main/tarefa17/grafico_tempo_vs_processos.png)

## 4. Conclusões

A escolha da distribuição por colunas impacta o desempenho de algumas maneiras:

- **Acesso à Memória (Cálculo Local)**: Ao contrário da matriz original, as porções `local_A` que cada processo recebe são contíguas em memória. Isso significa que, durante o cálculo, os acessos aos elementos de `local_A` são sequenciais, otimizando o uso do cache.

- **Comunicação da Matriz**: A distribuição da matriz A requer o uso de tipos de dados derivados para que o `MPI_Scatter` possa "coletar" as colunas dispersas na memória do processo raiz. Embora eficiente, há um overhead associado a essa operação.

- **Comunicação Final**: A agregação do vetor $y$ exige um `MPI_Reduce(MPI_SUM)`, onde todos os processos enviam seus vetores `local_y` para serem somados no processo raiz. O volume de dados nessa etapa é o tamanho do vetor $y$ ($M$ elementos).

### > Comparação com Distribuição por Linhas:

- Na distribuição por linhas, o `MPI_Scatter` para a matriz $A$ é geralmente mais direto, pois as linhas já são contíguas na memória em linguagens como C. No entanto, o vetor $x$ completo geralmente precisa ser broadcast para todos os processos, e o vetor $y$ é montado via `MPI_Gather`.
- O desempenho relativo de cada abordagem depende do tamanho da matriz ($M$ e $N$) e das características da rede (latência vs. largura de banda). Para problemas menores, o overhead de comunicação em ambas as estratégias pode dominar o tempo de execução. Para problemas maiores, os benefícios da paralelização se tornam mais evidentes.

Este projeto oferece uma base para entender os desafios e as soluções na paralelização de operações de álgebra linear com diferentes estratégias de distribuição de dados em `MPI`.