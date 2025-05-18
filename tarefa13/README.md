# Relatório 13 - Afinidade de threads
Aluna: Poliana Ellen de Araújo

## 1. Introdução
Este relatório apresenta a análise da escalabilidade de um código C para simulação de um campo de fluido baseado na equação de Navier-Strokes. O objetivo é investigar o impacto das diferentes configurações de afinidade de threads suportadas pelo OpenMP em um nó de computação do NPAD. As afinidades testadas foram: `false`, `true`, `close`, `spread` e `master`.

## 2. Metodologia

O código foi desenvolvido em C utilizando OpenMP para paralelismo. A simulação foi configurada para executar 1000 passos de tempo (NSTEPS) com um campo de 100x100 elementos (`nx` x `ny`). O método numérico utilizado foi o esquema de diferenças finitas para a solução da equação de Navier-Stokes considerando apenas o termo viscosidade.

A compilação foi realizada com as seguintes flags:
- `fopenmp`: habilitação do OpenMP.
- `O2`: otimização do código.

O código foi submetido ao sistema de filas utilizando o seguinte script em Shell (SLURM), o qual executou o programa para diferentes afinidades e número de threads.

Para compilar no NPAD:
```
sbatch strong_job.sh
```

## 3. Resultados

Os valores registrados incluem o número de threads, o tipo de afinidades (``OMP_PROC_BIND``), o tempo de execução, o valor no centro do campo (``CenterValue``) e a média dos valores no campo (``AverageValue``).

```
[pedarajo@service0 tarefa13]$ cat results_escalabilidade_forte.csv
Threads,nx,ny,Schedule,ChunkSize,Affinity,ExecutionTime,CenterValue,AverageValue
1,100,100,static,4,false,0.08548,0.09505,0.00010
2,100,100,static,4,false,0.06463,0.09505,-2161938778103065022432280576.00000
4,100,100,static,4,false,0.08248,0.09505,-2161938778103065022432280576.00000
8,100,100,static,4,false,0.09854,0.09505,-2161938778103065022432280576.00000
16,100,100,static,4,false,0.14983,0.09505,-2161939663546780560490758144.00000
1,100,100,static,4,true,0.08540,0.09505,0.00010
2,100,100,static,4,true,0.05539,0.09505,0.00010
4,100,100,static,4,true,0.08674,0.09505,0.00010
8,100,100,static,4,true,0.12877,0.09505,0.00010
16,100,100,static,4,true,0.17882,0.09505,0.00010
1,100,100,static,4,close,0.08552,0.09505,0.00010
2,100,100,static,4,close,0.05567,0.09505,0.00010
4,100,100,static,4,close,0.06806,0.09505,0.00010
8,100,100,static,4,close,0.09054,0.09505,0.00010
16,100,100,static,4,close,0.13241,0.09505,0.00010
1,100,100,static,4,spread,0.08584,0.09505,0.00010
2,100,100,static,4,spread,0.05590,0.09505,0.00010
4,100,100,static,4,spread,0.06824,0.09505,0.00010
8,100,100,static,4,spread,0.09151,0.09505,0.00010
16,100,100,static,4,spread,0.13304,0.09505,0.00010
1,100,100,static,4,master,0.08559,0.09505,0.00010
2,100,100,static,4,master,3.69413,0.09505,0.00010
4,100,100,static,4,master,0.10323,0.09505,0.00010
8,100,100,static,4,master,0.14420,0.09505,-2943295684608.00000
16,100,100,static,4,master,0.18838,0.09505,-2943833866240.00000
```
### 3.1. Tempo de execução
- Afinidade ``false``
    - O tempo de execução aumenta de forma não linear conforme o número de threads aumenta. Isso é um comportamento inesperado, pois a escalabilidade forte deveria reduzir o tempo de execução ou, no mínimo mantê-lo.

## 4. Conclusões