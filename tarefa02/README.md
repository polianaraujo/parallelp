# Análise de Paralelismo ao Nível de Instrução (ILP) em C

Este projeto demonstra o impacto do Paralelismo ao Nível de Instrução (ILP) no desempenho de código C, analisando a interação entre o **estilo de codificação** e os **níveis de otimização do compilador** (GCC).

## Metodologia

O experimento mede e compara o tempo de execução de três funções em C:

1.  `initialize_array`: Um laço com iterações **independentes** (alto potencial de ILP).
2.  `sum_sequential`: Um laço com **dependência de dados** entre as iterações, o que limita o ILP.
3.  `sum_parallelized`: Uma versão do laço anterior, mas reescrita para **quebrar a dependência de dados** usando múltiplos acumuladores.

Cada versão foi compilada com os níveis de otimização do GCC: `-O0` (sem otimização), `-O2` (otimização padrão) e `-O3` (otimização agressiva).

## Resultados Principais

![Gráfico do tempo de execução por otimização](https://github.com/polianaraujo/parallelp/blob/main/tarefa02/grafico_desempenho.png)

1.  **Otimização do Compilador é Crucial**: A mudança de `-O0` (sem otimização) para `-O2` resultou em uma aceleração massiva, chegando a **5.4 vezes** no caso do código com dependência de dados.

2.  **Design de Código Importa**: Escrever código para quebrar dependências de dados é altamente eficaz. A versão `sum_parallelized` foi **2.55 vezes mais rápida** que a `sum_sequential` quando compiladas sem otimizações (`-O0`).

3.  **Compiladores Modernos são Inteligentes**: Com as otimizações `-O2` e `-O3` ativadas, o compilador conseguiu reduzir o impacto da dependência de dados, diminuindo a vantagem do código otimizado manualmente. Ainda assim, a versão com melhor design (`sum_parallelized`) permaneceu mais rápida em todos os testes.

4.  **Otimização Máxima Nem Sempre é Melhor**: O nível `-O3` não garantiu um desempenho superior ao `-O2`. Em um dos testes, o código executou ligeiramente mais lento com `-O3`, mostrando a importância de testar (fazer *profiling*) em vez de assumir que a otimização máxima é sempre a melhor escolha.

## Conclusão

Para obter o máximo desempenho, a melhor abordagem é combinar as duas estratégias:

  * **Escrever código que exponha o paralelismo**, minimizando dependências de dados.
  * **Utilizar e testar os níveis de otimização do compilador** para encontrar a melhor configuração para a sua aplicação.

## Como Executar

Para compilar e executar o projeto:

```bash
./run.sh
```