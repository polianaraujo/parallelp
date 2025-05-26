# Relatório 12 - Avaliação da Escalabilidade
Aluna: Poliana Ellen de Araújo

## 1. Introdução

Este trabalho tem como objetivo avaliar a escalabilidade de um código baseado nas equações de Navier-Stokes utilizando a infraestrutura de computação de alto desempenho (HPC) do NPAD da UFRN. A proposta é identificar gargalos de desempenho e evoluir o código com técnicas de paralelismo, analisando os impactos nas métricas de escalabilidade forte e fraca.

## 2. Metodologia

O código utilizado resolve numericamente as equações de Navier-Stokes bidimensionais, aplicando o método das diferenças finitas. A paralelização foi feita utilizando a API OpenMP, explorando o paralelismo em laços onde operações independentes são realizadas sobre elementos da malha de solução.

Os arquivos principais estão no diretório raiz, e são o main_forte.c e main_fraca.c, focadas na análise de escalabilidade forte e fraca, a primeira mantendo o número NX e NY ao passo de diferentes quantidades de threads, e o último aumentando proporcionalmente o tamanho do problema com o número de threads.

Para submeter no NPAD:
```
sbatch shell_forte.sh
sbatch shell_forte.sh
```

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