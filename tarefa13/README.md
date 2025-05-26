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


|Comparação das afinidades de todas as threads|Tempo vs threads por afinidade|
|-----|-----|
|![Comparação das afinidades de todas as threads](https://github.com/polianaraujo/parallelp/blob/main/tarefa13/graf_afinidade/comparacao_afinidades_todas_threads.png)|![Tempo vs threads por afinidade](https://github.com/polianaraujo/parallelp/blob/main/tarefa13/graf_afinidade/tempo_vs_threads_por_afinidade.png)|

Os resultados mostraram uma tendÊncia de redução de tempo de execução com o aumento de threads, especialmente até 16 threads.

- Com 32 threads, as afinidades `true`, `spread` e `close` apresentaram menores tempos de execução, com destaque para `true` com 0,1427s.
- A afinidade `master`, por outro lado, apresentou degradação severa do desempenho, chegando a 5,4353s com 32 threads.
- As afinidades `spread` e `true` foram as mais consistentes, mantendo tempos baixos e estáveis conforme o número de threads aumentava.
- O tempo com uma thread (serial) ficou entre 0,91s e 0,97s, servindo como referência para o cálculo de speedup.

Os gráficos evidenciam a importância da escolha adequada da afinidade no desempenho paralelo.

## 4. Conclusões

A análise demonstrou que a configuração da afinidade de threads tem impacto direto na escalabilidade e desempenho de programas paralelizados com OpenMP, e as principais conclusões são:
- A utilização de múltiplas threads melhora significativamente o tempo de execução, especialmente com as afinidades `true` e `spread`.
- A afinidade `master` é desaconselhada para execuções paralelas com mais de 4 threads, em razão da limitação de alocação de threads em um único número ou socket.
- Para aplicações similares executadas em ambientes HPC como o NPAD, a escolha criteriosa da afinidade pode representar uma economia significativa de tempo computacional, principalmente em execuções de larga escala.