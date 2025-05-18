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
1,100,100,static,4,false,0.02085,nan,nan
2,100,100,static,4,false,0.08044,nan,nan
4,100,100,static,4,false,0.09409,nan,nan
8,100,100,static,4,false,0.11257,nan,nan
16,100,100,static,4,false,0.14605,nan,nan
1,100,100,static,4,true,0.09296,0.09505,-6140464726016.00000
2,100,100,static,4,true,0.11170,0.09505,-352858876674048.00000
4,100,100,static,4,true,0.15510,0.09505,-352841797468160.00000
8,100,100,static,4,true,0.19413,0.09505,-356298239508480.00000
16,100,100,static,4,true,0.24593,0.09505,-356297937518592.00000
1,100,100,static,4,close,0.08769,0.09505,0.00010
2,100,100,static,4,close,0.14002,0.09505,0.00010
4,100,100,static,4,close,0.15597,0.09505,0.00010
8,100,100,static,4,close,0.17591,0.09505,0.00010
16,100,100,static,4,close,0.20541,0.09505,0.00010
1,100,100,static,4,spread,0.09178,0.09505,6663913318225339744256.00000
2,100,100,static,4,spread,0.10813,0.09505,382939779278620592177152.00000
4,100,100,static,4,spread,0.12028,0.09505,382921224448155825733632.00000
8,100,100,static,4,spread,0.13975,0.09505,386672506764973335117824.00000
16,100,100,static,4,spread,0.17300,0.09505,386672146477003145478144.00000
1,100,100,static,4,master,0.08737,0.09505,0.00010
2,100,100,static,4,master,3.32560,0.09505,0.00010
4,100,100,static,4,master,0.20230,0.09505,0.00010
8,100,100,static,4,master,0.21272,0.09505,0.00010
16,100,100,static,4,master,0.25192,0.09505,0.00010
```
### 3.1. Tempo de execução
- Afinidade ``false``
    - O tempo de execução aumenta de forma não linear conforme o número de threads aumenta. Isso é um comportamento inesperado, pois a escalabilidade forte deveria reduzir o tempo de execução ou, no mínimo mantê-lo.

## 4. Conclusões