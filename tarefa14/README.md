# Relatório 14 - Latência de comunicação usando MPI
Aluna: Poliana Ellen de Araújo

## 1. Introdução

Este experimento avalia a latência de comunicação entre dois processos MPI por meio de trocas de mensagens (ping-pong). Foram utilizados tamanhos variando de 8 bytes a 1 MB, com 1000 iterações por tamanho. O tempo de cada troca foi medido com MPI_Wtime() e os resultados foram salvos em um arquivo CSV para posterior análise gráfica.


## 2. Metodologia

- O programa é executado com exatamente 2 processos MPI: um envia e o outro recebe.

- A verificação de quantidade de processos é essencial para:

    - Evitar comunicação incorreta ou deadlocks.

    - Garantir resultados consistentes.

- Para cada tamanho de mensagem, é realizada a troca de mensagens ida e volta entre os processos.

- O tempo total e médio é calculado e registrado.


## 3. Resultados

Gráficos gerados a partir do CSV com os tempos de execução:

|Comparação das afinidades de todas as threads|Tempo vs threads por afinidade|
|-----|-----|
|![Comparação das afinidades de todas as threads](https://github.com/polianaraujo/parallelp/blob/main/tarefa14/images/tempo_medio_vs_tamanho.png)|![Tempo vs threads por afinidade](https://github.com/polianaraujo/parallelp/blob/main/tarefa14/images/tempo_total_vs_tamanho.png)|


## 4. Conclusões

- Para tamanhos pequenos de mensagem, o tempo médio permanece quase constante, dominado pela latência da comunicação.

- A partir de ~4KB, observa-se crescimento no tempo, indicando que a largura de banda passa a ser o fator dominante.

- A análise mostra a importância de considerar o perfil de comunicação em aplicações paralelas para otimizar desempenho.