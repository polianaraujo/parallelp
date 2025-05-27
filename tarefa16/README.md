# Relatório 16 - Comunicação Coletiva com MPI
Aluna: Poliana Ellen de Araújo

## 1. Introdução


## 2. Metodologia



## 3. Resultados




![Comparação das afinidades de todas as threads](https://github.com/polianaraujo/parallelp/blob/main/tarefa16/tempo_vs_processos_tamanhos_matrizes.png)

## 4. Conclusões

- 📈 Tendências observadas
    - ✅ Eficiência do paralelismo:
        - A paralelização melhora o desempenho até certo ponto.
        - O número ótimo de processos varia com o tamanho do problema (M e N).

- ❌ Overhead de comunicação:
    - Quando M ou N são pequenos, muitos processos introduzem mais overhead do que benefício.
    - Quando M é grande o suficiente, o paralelismo compensa o overhead — mas até um certo ponto.


- Problemas pequenos (ex: M=1000, N=100): poucos processos já são suficientes; muitos processos não trazem ganho significativo.

- Problemas médios (ex: M=10000, N=1000): 4 a 8 processos são eficazes, mas deve-se monitorar a sobrecarga.

- Problemas grandes (ex: M=50000, N=1000): 4 processos parecem ser o ponto de equilíbrio; 8 processos aumentam a comunicação e não compensam em desempenho.