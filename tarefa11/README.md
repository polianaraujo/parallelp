# Relatório 11 - Impacto das cláusulas schedule e collapse
Aluna: Poliana Ellen de Araújo


## 1. Introdução

Este relatório apresenta uma **simulação da equação de Navier-Stokes** considerando apenas a viscosidade, sem pressão ou forças externas. A simulação utiliza **diferenças finitas** para discretizar o espaço e acompanhar a evolução da velocidade do fluido ao longo do tempo.

Inicialmente, o fluido foi configurado em repouso ou com velocidade constante, seguido da introdução de uma pequena perturbação para observar a estabilidade do campo.

Após a validação, o código foi **paralelizado com OpenMP**, avaliando o impacto das cláusulas `schedule` e `collapse` no desempenho.

Foram realizados dois conjuntos de experimentos:

* **Com perturbação (versão sequencial):** para análise visual do comportamento da simulação.
* **Sem perturbação (versão paralela):** para testes de desempenho, sem interferência de valores iniciais não-nulos.

A equação da viscosidade (sem pressão e forças externas) em duas dimensões é dada por:

$$ \frac{𝜕u}{𝜕t}=v𝛻^{2}u $$

Onde:
- $u$: Campo de velocidade do fluido
- $v$: Coeficiente de viscosidade
- $𝛻^{2}$: Operador Laplaciano

![navier_stokes_diffusion](navier_stokes_diffusion.gif)


## 2. Metodologia

### 2.1. Código Sequencial - Equação considerando apenas a viscosidade
- Definição de Constantes:
    - `NX` e `NY`: Dimensões da grade, nesse caso `100x100`.
    - `NSTEPS`: Número de passos de tempo, nesse caso `1000`.
    - `DT`: Passo de tempo, nesse caso `0,1`.
    - `VISCOSITY`: Coeficiente de viscosidade, nesse caso `0,1`.
- Função ``initialize_field``:
    - Inicializa o campo de velocidade com zeros.
    - Introduz uma perturbação no centro da grade.
- Função ``update_field``:
    - Calcula a próxima iteração do campo (como ele se comporta com o tempo) usando a fórmula discreta da equação de Navier-Stokes.
- Função ``simulate``:
    - Realiza a simulação iterativa, em `NSTEPS`.
    - Em cada iteração, a função update_field é chamada para atualizar o campo (simulação de como a velocidade do fluido, em cada ponto da malha, muda ao longo do tempo, considerando apenas os efeitos da difusão viscosa).
- Função ``print_field``:
    - Imprime o campo em formato simplificado para monitoramento.
- Função ``main``:
    - Inicializa o campo, executa a simulação calculando o tempo através da função `gettimeofday()`.


### 2.2. Código Paralelo - Com OpenMP

A simulação foi paralelizada com OpenMP para explorar múltiplos threads e melhorar o desempenho. As regiões paralelizadas foram:

* Cálculo do laplaciano interno da matriz.
* Cópia da matriz `next` para `current` a cada passo de tempo.

O código foi executado **com e sem perturbação** (linha 21 comentada).

Foram testadas:

* Estratégias de escalonamento: `static`, `dynamic` e `guided`.
* Tamanhos de chunk: 1, 4 e 8.
* Números de threads: 1, 2, 4, 8 e 16.

A inicialização foi feita com o campo zerado para garantir consistência. O tempo de execução foi medido com `omp_get_wtime()`.

**Modificações no código:**

* Inclusão de `#include <omp.h>`.
* Na função `update_field`, foi usada a diretiva:

  ```c
  #pragma omp parallel for collapse(2) schedule(static)
  ```

  * `parallel for`: paraleliza o loop.
  * `collapse(2)`: unifica os loops `i` e `j`.
  * `schedule(...)`: define a estratégia de distribuição (também testadas `dynamic` e `guided`).

A função `run_simulation` permite executar a simulação variando:

* Número de threads.
* Tipo de escalonamento.
* Tamanho do chunk.

Essas variações foram usadas para analisar o impacto da paralelização no desempenho.

### 2.3. Execução

```
gcc -o sequential sequential.c -lm
./sequential
gcc -o parallel parallel.c -fopenmp -lm
./parallel
```

## 3. Resultados

Enquanto os dados da execução sem perturbação (transformando a linha acima comentada), foram salvos em um arquivo chamado results_sem_perturbacao.csv.

|Sem perturbação|Com perturbação|
|-----|-----|
|![combined_analysis_sem_perturb](https://github.com/polianaraujo/parallelp/blob/main/tarefa11/plots_sem_perturb/combined_analysis_sem_perturb.png)|![Combinação](https://github.com/polianaraujo/parallelp/blob/main/tarefa11/plots_w_perturb/combined_analysis_w_perturb.png)|
|![Combinação](https://github.com/polianaraujo/parallelp/blob/main/tarefa11/plots_sem_perturb/time_per_chunk_sem_perturb.png)|![Combinação](https://github.com/polianaraujo/parallelp/blob/main/tarefa11/plots_w_perturb/time_per_chunk_w_perturb.png)|

### 🔹 3.1. Sem perturbação

- O escalonamento guided apresentou o melhor desempenho geral, principalmente com 2, 4 e 8 threads.

- O dynamic mostrou maior variabilidade e, em alguns casos, pior desempenho.

- Com 16 threads, todos os agendamentos apresentaram piora no tempo de execução, indicando excesso de paralelismo.

- O tempo de execução diminui com o aumento de threads até 8, sendo esse o ponto ótimo.

- Chunk size menor (1) gera maior tempo, devido ao overhead de agendamento.

- Chunk sizes 4 e 8 são mais eficientes, especialmente com escalonamento guided.

- No modo static, a variação com o chunk size é pequena.

### 🔹 3.2. Com perturbação

- O comportamento geral se mantém semelhante ao caso sem perturbação, com o guided se destacando em eficiência.

- O uso de 16 threads continua sendo ineficiente por conta da sobrecarga.

- O chunk size 1 é mais afetado por perturbações, resultando em maior tempo de execução.

- Chunk sizes 4 e 8 seguem mais robustos, principalmente no escalonamento static.

## 4. Conclusões

* O escalonamento `guided` teve o melhor desempenho, especialmente com até 4 threads.
* `Dynamic` mostrou baixa eficiência em cenários com muitas threads, sendo pouco indicado para escalabilidade fraca.
* O melhor desempenho geral foi com até 4 threads; acima disso, o overhead compromete os ganhos.
* A cláusula `collapse=1` foi, em geral, mais eficiente com muitas threads.
* O paralelismo deve ser ajustado ao tamanho da carga: muitos threads em problemas pequenos reduzem a eficiência.
* A escolha ideal combina política de escalonamento, número de threads e uso de `collapse`, sendo `static` ou `guided` mais indicados para cargas maiores.
