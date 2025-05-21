import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Configurações visuais
sns.set(style="whitegrid")
plt.figure(figsize=(12, 8))

# Lê os dados do CSV
df = pd.read_csv("resultados.csv")

# Ordena para garantir consistência nos eixos
df = df.sort_values(by=["M", "N", "Num_processos"])

# Cria uma coluna combinando M e N para usar como legenda
df["Dimensão"] = df.apply(lambda row: f"M={row.M}, N={row.N}", axis=1)

# Plota todas as curvas no mesmo gráfico
sns.lineplot(
    data=df,
    x="Num_processos",
    y="Tempo_segundos",
    hue="Dimensão",
    marker="o"
)

plt.title("Tempo de execução vs. Número de processos")
plt.xlabel("Número de processos")
plt.ylabel("Tempo (segundos)")
plt.xticks(sorted(df["Num_processos"].unique()))
plt.legend(title="Tamanho da matriz")
plt.grid(True)
plt.tight_layout()

# Salva a imagem
plt.savefig("grafico_tempo_vs_processos.png")
plt.show()
