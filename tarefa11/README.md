# Relatório 11 - Impacto das cláusulas schedule e collapse
Aluna: Poliana Ellen de Araújo

## 1. Introdução

Este relatório apresenta uma **simulação do movimento de um fluido ao longo do tempo usando a equação de Navier-Stokes**, considerando apenas os efeitos da viscosidade, desconsiderando a pressão e quaisquer forças externas. Utilizando **diferenças finitas para discretizar o espaço e simule a evolução da velocidade do fluido no tempo**. Foi pedido para inicializar o fluido parado ou com velocidade constante e verificar se o campo permanece estável, e em seguida inserido uma pequena perturbação. Após validar, o código foi paralelizado com OpenMP explorando o impacto das cláusulas schedule e collapse no desempenho da execução paralela.
Portante, foram realizados dois conjuntos distintos de experimentos:
- Com Perturbação Inicial (versão sequencial): utilizada para visualização do comportamento da simulação ao longo do tempo.
- Sem Perturbação Inicial (versão paralela): utilizada para testes de desempenho isolados, eliminando efeitos de valores iniciais não-nulos.


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

Paralelizar a simulação utilizando OpenMP para explorar múltiplos threads e melhorar o desempenho. O paralelismo foi aplicado nas seguintes regiões:
- Cálculo do laplaciano interno da matriz.
- Cópia da matriz next para current a cada passo temporal.
O código foi executado **com e sem perturbação**(linha 21 comentada).

Três tipos de escalonamento foram testados:
- ``static``
- `dynamic`
- `guided`

Com três tamanhos de chunk: 1, 4 e 8. O número de threads variou entre 1, 2, 4, 8 e 16. Para garantir consistência nas medições, a inicialização foi feita sem perturbação (campo inicializado com zeros). O tempo de execução foi medido com `omp_get_wtime()`.

- Inclusão da Biblioteca OpenMP:
    - `#include <omp.h>`
- Ajuste da Função `update_field`:
    - A diretiva `#pragma omp parallel for collapse(2)` `schedule(static)` foi adicionada.
        - `parallel for`: Indica que o loop será paralelizado.
        - `collapse(2)`: Unifica os dois loops aninhados (i e j) em um único loop virtual, permitindo um balanceamento mais eficiente da carga de trabalho.
        - `schedule(static)`: Divide o loop em blocos iguais e atribui cada bloco a um thread (`dynamic` e `guided` são outras opções, que ajustam a divisão dinamicamente ou de forma guiada).

Para avaliar o impacto da paralelização no desempenho da simulação, foi implementado no código a função `run_simulation`, que permite executar a simulação com diferentes configurações:
- Variação do número de threads (1, 2, 4, 8, 16), com o intuito de investigar o efeito do grau de paralelismo.
- Diferentes tipos de escalonamento (`static`, `dynamic`, `guided`), servindo para determinar como as interações do loop paralelo são distruibuídas entre as threads
- Diferentes tamanhos de chunk (1, 4, 8), definindo a granularidade das tarefas atribuídas a cada thread.

### 2.3. Execução

A execução foi realizada com os seguintes comandos, para o sequencial e o paralelo, respectivamente:

```
gcc -o sequential sequential.c -lm
./sequential
gcc -o parallel parallel.c -fopenmp -lm
./parallel
```

A versão paralela foi executada duas vezes, a primeira com a aplicação de perturbação, através da linha 21 abaixo, e o resultado foi salvo em um arquivo csv chamado results_w_perturbacao.csv:
```
field[NX / 2][NY / 2] = 1.0f;
```

## 3. Resultados

Enquanto os dados da execução sem perturbação (transformando a linha acima comentada), foram salvos em um arquivo chamado results_sem_perturbacao.csv.

Em ambos, os dados foram coletados contém:
- Número de threads utilizadas
- Tipo de escalonamento
- Tamanho do chunk
- Tempo total de execução
- Valor final no centro da matriz
- Valor médio de toda a matriz

|Sem perturbação|Com perturbação|
|-----|-----|
|![combined_analysis_sem_perturb](https://github.com/polianaraujo/parallelp/blob/main/tarefa11/plots_sem_perturb/combined_analysis_sem_perturb.png)|![Combinação](https://github.com/polianaraujo/parallelp/blob/main/tarefa11/plots_w_perturb/combined_analysis_w_perturb.png)|
|![Combinação](https://github.com/polianaraujo/parallelp/blob/main/tarefa11/plots_sem_perturb/time_per_chunk_sem_perturb.png)|![Combinação](https://github.com/polianaraujo/parallelp/blob/main/tarefa11/plots_w_perturb/time_per_chunk_w_perturb.png)|

### 3.1 Sem perturbação

🔹 `combined_analysis_sem_perturb`: Threads × Schedule × Tempo
Este gráfico compara os tempos de execução em função da quantidade de threads e do tipo de escalonamento utilizado (static, dynamic, guided). De modo geral, observam-se as seguintes tendências:

Melhor desempenho (menor tempo) é obtido com o escalonamento guided, especialmente quando o número de threads aumenta até 8. Para 2, 4 e 8 threads, guided apresenta tempos significativamente menores que os demais tipos.

O escalonamento dynamic apresenta maior variabilidade, e em alguns casos (como com 4 e 8 threads), resulta em tempos superiores aos outros métodos.

Com 16 threads, todos os tipos de escalonamento apresentam um aumento abrupto no tempo de execução, provavelmente devido à sobrecarga de paralelismo e/ou limitação de recursos computacionais. Isso sugere ineficiência na utilização de muitos threads para esse problema específico.

O tempo de execução diminui gradualmente com o aumento do número de threads até 8, e volta a crescer com 16 threads, indicando um ponto ótimo próximo de 4 a 8 threads, dependendo do escalonamento.

🔹 `time_per_chunk_sem_perturb`: Chunk Size × Tempo
Neste gráfico, analisamos o tempo de execução em relação ao tamanho do chunk (1, 4, 8) para cada combinação de threads e tipo de escalonamento.

Para a maioria dos casos, um tamanho de chunk menor (1) resulta em maior tempo de execução, especialmente com escalonamento dynamic. Isso se deve ao overhead de agendamento muito frequente.

Com o aumento do tamanho do chunk (4 e 8), o tempo tende a reduzir até certo ponto, pois o agendamento ocorre com menos frequência.

No escalonamento guided, os tempos são consistentemente baixos independentemente do chunk size, o que demonstra a eficiência adaptativa deste tipo de escalonamento.

No escalonamento static, a diferença entre tamanhos de chunk é pequena, o que é esperado, visto que a divisão de trabalho é definida previamente e não muda durante a execução.

### 3.2 Com perturbação

🔹 `combined_analysis_com_perturb`: Threads x Schedule x Tempo
Este gráfico permite comparar o tempo de execução para diferentes combinações de número de threads e estratégias de escalonamento (static, dynamic e guided) com chunk sizes variados, em um cenário com perturbações.

Observa-se que, para até 8 threads, o tempo de execução tende a reduzir com o aumento do número de threads, especialmente nos agendamentos guided e static.

Para 16 threads, todos os escalonamentos apresentam um tempo de execução mais alto e relativamente estável, indicando possível overhead de paralelismo em excesso ou contenção de recursos, comum em sistemas com muitos threads competindo simultaneamente.

O agendamento guided mostrou o melhor desempenho geral para 4 e 8 threads, mantendo tempos de execução baixos e consistentes.

O escalonamento dynamic teve desempenho inferior para 1 e 2 threads, mas se aproximou do desempenho dos outros à medida que as threads aumentaram.

Em todos os agendamentos, o chunk size afeta pouco o desempenho para guided, mas causa maior variação nos modos static e dynamic.

🔹 `time_per_chunk_com_perturb`: Tempo por Chunk Size
Este gráfico mostra o tempo médio de execução por chunk size (1, 4 e 8), agrupando todos os valores de threads e escalonamentos.

Observa-se uma tendência de queda no tempo médio de execução com o aumento do chunk size. Isso é esperado, pois maiores chunk sizes reduzem a sobrecarga de agendamento.

Chunk size 1 apresenta o maior tempo médio, pois exige agendamento mais frequente e granular, sendo mais sensível às perturbações.

Chunk sizes 4 e 8 são mais eficientes e robustos à perturbação, especialmente no modo static.


## 4. Conclusões

- A política de escalonamento `guided` apresentou os melhores resultados de tempo para escalabilidade forte, especialmente com baixo número de threads (1 a 4).
- A política `dynamic` não é adequada para escalabilidade fraca, pois demonstrou altos tempos de execução e baixa eficiência, especialmente com maiores quantidades de threads.
- A melhor performance global foi alcançada com até 4 threads, sendo que, a partir disso, o overhead de gerenciamento de threads parece superar o ganhos de paralelismo.
- A cláusula `collapse` teve impacto variável, mas em geral, valores mais baixos (`collapse=1`) apresentaram melhor desempenho em cenários com muitas threads.
- A análise sugere que o uso de paralelismo deve ser balanceado com o tamanho da carga de trabalho, evitando o uso excessivo de threads em problemas pequenos.
- O desempenho ótimo depende da combinação entre política de escalonamento, número de threads e estrutura de laços (`collapse`). Para cargas maiores, a política `static` ou `guided` com ajustes adequados pode ser a mais indicada.
