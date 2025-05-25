# Relatório 11 - Impacto das cláusulas schedule e collapse
Aluna: Poliana Ellen de Araújo

## 1. Introdução

Este relatório apresenta uma **simulação do movimento de um fluido ao longo do tempo usando a equação de Navier-Stokes**, considerando apenas os efeitos da viscosidade, desconsiderando a pressão e quaisquer forças externas. Utilizando **diferenças finitas para discretizar o espaço e simule a evolução da velocidade do fluido no tempo**. Foi pedido para inicializar o fluido parado ou com velocidade constante e verificar se o campo permanece estável, e em seguida inserido uma pequena perturbação. Após validar, o código foi paralelizado com OpenMP explorando o impacto das cláusulas schedule e collapse no desempenho da execução paralela.
Portante, foram realizados dois conjuntos distintos de experimentos:
- Com Perturbação Inicial (versão sequencial): utilizada para visualização do comportamento da simulação ao longo do tempo.
- Sem Perturbação Inicial (versão paralela): utilizada para testes de desempenho isolados, eliminando efeitos de valores iniciais não-nulos.

\frac{u\power-index{n+1|i,j}-u\power-index{n|i,j}|Δt}=v\frac{u\power-index{n|i+1,j}-u\power-index{n|i-1,j}+u\power-index{n|i,j+1}+u\power-index{n|i,j-1}-4n\power-index{n|i,j}|Δx\power{2}}

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
    - Calcula a próxima iteração do campo usando a fórmula discreta da equação de Navier-Stokes.
- Função ``simulate``:
    - Realiza a simulação iterativa.
    - Em cada iteração, a função update_field é chamada para atualizar o campo.
- Função ``print_field``:
    - Imprime o campo em formato simplificado para monitoramento.
- Função ``main``:
    - Inicializa o campo, executa a simulação e exibe os resultados.

A implementação sequencial foi feita em linguagem C, utilizando funções padrão de manipulação de arrays. O tempo de execução é medido com gettimeofday(). O campo simulado é inicializado com uma perturbação central unitária (valor 1.0), e a matriz é atualizada iterativamente.

### 2.2. Código Paralelo - Com OpenMP

Paralelizar a simulação utilizando OpenMP para explorar múltiplos threads e melhorar o desempenho. O paralelismo foi aplicado nas seguintes regiões:
- Cálculo do laplaciano interno da matriz.
- Cópia da matriz next para current a cada passo temporal.

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

Enquanto os dados da execução sem perturbação (transformando a linha acima comentada), foram salvos em um arquivo chamado results_sem_perturbacao.csv.

Em ambos, os dados foram coletados contém:
- Número de threads utilizadas
- Tipo de escalonamento
- Tamanho do chunk
- Tempo total de execução
- Valor final no centro da matriz
- Valor médio de toda a matriz

## 3. Resultados

|Escalabilidade forte|Escalabilidade fraca|
|-----|-----|
|![Escalabilidade forte](https://github.com/polianaraujo/parallelp/blob/main/tarefa12/graficos/escalabilidade_forte.png)|![Escalabilidade fraca](https://github.com/polianaraujo/parallelp/blob/main/tarefa12/graficos/escalabilidade_fraca.png)|

### 3.1. Escalabilidade Forte
Com o tamanho do problema fixo (`N = 128`), podendo observar:

- Com 1 thread, o tempo de execução varios entre ~1.2 e 1.7 segundos, dependendo da política de escalonamento e da cláusula `collaspse`. O melhor tempo foi com `guided` e `caollpase=3`.
- A redução no tempo de execução foi notável com o aumento de threads até certo ponto. Com 2 e 4 threads, o tempo caiu consideralvemente, alcançando valores em torno de 0.27s e 0.68s.
- Com 8 threads, houve bons resultados com `guided` e `dynamic`, especialmente para `collapse=1`, com tempos próximos a 0.28s.
- A partir de 16 threads, observou-se uma diminuição nos ganhos de desempenho. Em alguns casos, como `collapse=3`, o tempo voltou a aumentar.
- Com `128` threads, o desempenho piorou, com tempos que se aproximaram novamente de 1.4-1.7s, sugerindo overhead de paralelização.

### 3.2. Escalabilidade Fraca

O tamanho do problema cresce proporcionalmente ao número de threads.
- Com 1 thread e `N=50`, o tempo foi cerca de 0.27s para todas as políticas e valores de `collapse`, exceto `dynamic`, que apresentou valores próximos a 0.79s.
- Com 2 threads (`N=100`), os tempos aumentaram para ~`1.56s` para `static` e `guided`, e ultrapassaram 5s com `dynamic`, mostrando um desempenho ruim com esse política.
- Com 4 threads (`N=200`), `static` e `guided` mantiveram tempos ao redor de 13s, enquanto `dynamic` foi significativamente maior, chegando a 42s.
- Com 8 threads (`N=400`), os tempos com `static` subiram para mais de `100s`, enquanto `dynamic` atingiu pucos acima de 340s, evidenciando grande ineficiência.

## 4. Conclusões

- A política de escalonamento `guided` apresentou os melhores resultados de tempo para escalabilidade forte, especialmente com baixo número de threads (1 a 4).
- A política `dynamic` não é adequada para escalabilidade fraca, pois demonstrou altos tempos de execução e baixa eficiência, especialmente com maiores quantidades de threads.
- A melhor performance global foi alcançada com até 4 threads, sendo que, a partir disso, o overhead de gerenciamento de threads parece superar o ganhos de paralelismo.
- A cláusula `collapse` teve impacto variável, mas em geral, valores mais baixos (`collapse=1`) apresentaram melhor desempenho em cenários com muitas threads.
- A análise sugere que o uso de paralelismo deve ser balanceado com o tamanho da carga de trabalho, evitando o uso excessivo de threads em problemas pequenos.
- O desempenho ótimo depende da combinação entre política de escalonamento, número de threads e estrutura de laços (`collapse`). Para cargas maiores, a política `static` ou `guided` com ajustes adequados pode ser a mais indicada.
