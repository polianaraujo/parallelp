# Relatório 13 - Afinidade de threads
Aluna: Poliana Ellen de Araújo

## 1. Introdução
Este relatório apresenta a análise da escalabilidade de um código C para simulação de um campo de fluido baseado na equação de Navier-Strokes. O objetivo é investigar o impacto das diferentes configurações de afinidade de threads suportadas pelo OpenMP em um nó de computação do NPAD.

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

Os experimentos foram realizados no ambiente do NPAD, executando um código paralelo que resolve as equações de Navier-Stokes utilizando OpenMP. Avaliou-se o tempode execução variando:
- O número de threads (1, 2, 4, 8, 16, 32)
- A política de afinidade de threads (`false`, `true`, `close`, `spread`, `master`)

```
[pedarajo@service0 tarefa13]$ cat results_20250518_160541.csv
threads,affinity,time
1,false,0.958370
1,true,0.933981
1,close,0.973994
1,spread,0.917426
1,master,0.918329
2,false,0.538263
2,true,0.522186
2,close,0.503780
2,spread,0.550748
2,master,0.849626
4,false,0.347166
4,true,0.324282
4,close,0.348438
4,spread,0.323327
4,master,1.180983
8,false,0.240428
8,true,0.234981
8,close,0.241779
8,spread,0.237994
8,master,1.784695
16,false,0.180741
16,true,0.168707
16,close,0.169054
16,spread,0.178430
16,master,2.973133
32,false,0.147721
32,true,0.142729
32,close,0.145915
32,spread,0.144840
32,master,5.435383
```

|omparação das afinidades de todas as threads|Tempo vs threads por afinidade|
|-----|-----|
|![Comparação das afinidades de todas as threads](https://github.com/polianaraujo/parallelp/blob/main/tarefa13/graf_afinidade/comparacao_afinidades_todas_threads.png)|![Tempo vs threads por afinidade](https://github.com/polianaraujo/parallelp/blob/main/tarefa13/graf_afinidade/tempo_vs_threads_por_afinidade.png)|

Os resultados mostraram uma tendÊncia de redução de tempo de execução com o aumento de threads, especialmente até 16 threads.

- Com 32 threads, as afinidades `true`, `spread` e `close` apresentaram menores tempos de execução, com destaque para `true` com 0,1427s.
- A afinidade `master `

### 3.1. Tempo de execução
- Afinidade ``false``
    - O tempo de execução aumenta de forma não linear conforme o número de threads aumenta. Isso é um comportamento inesperado, pois a escalabilidade forte deveria reduzir o tempo de execução ou, no mínimo mantê-lo.

## 4. Conclusões