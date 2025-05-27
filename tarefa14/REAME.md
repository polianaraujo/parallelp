# Relatório 14 - Latência de comunicação usando MPI
Aluna: Poliana Ellen de Araújo

## 1. Introdução
- Objetivo do Programa
    - Dois processos se comunicam entre si trocando uma mensagem de ida e volta (ping-pong).

    - Vários tamanhos de mensagem são testados (de 8 bytes a 1 MB).

    - Múltiplas iterações (1000 por tamanho) são usadas para obter uma média confiável.

    - O tempo é medido com MPI_Wtime() e armazenado em um CSV para análise gráfica posterior.



## 2. Metodologia

Garantir que o há exatamente 2 processos sendo executados. Se haver mais ou menos, quer dizer que:
- Pode haver comunicação mal direcionada.
- Pode causar **deadlock**.
- Ou simplesmente gerar resultados incorretos.


## 3. Resultados



|Comparação das afinidades de todas as threads|Tempo vs threads por afinidade|
|-----|-----|
|![Comparação das afinidades de todas as threads](https://github.com/polianaraujo/parallelp/blob/main/tarefa13/graf_afinidade/comparacao_afinidades_todas_threads.png)|![Tempo vs threads por afinidade](https://github.com/polianaraujo/parallelp/blob/main/tarefa13/graf_afinidade/tempo_vs_threads_por_afinidade.png)|



## 4. Conclusões

