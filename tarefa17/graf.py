# graf.py

import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os

# Configurações de estilo
sns.set(style="whitegrid")
plt.rcParams.update({"figure.autolayout": True})

# Carregar o CSV
df = pd.read_csv("resultados.csv")

# Verifica se as colunas esperadas estão presentes
assert all(col in df.columns for col in ["M", "N", "Num_processos", "Tempo_segundos"])

# Criar pasta de saída
output_dir = "graficos"
os.makedirs(output_dir, exist_ok=True)

# Agrupar por combinações de (M, N)
grupos = df.groupby(["M", "N"])

for (M, N), grupo in grupos:
    plt.figure(figsize=(8, 5))
    sns.lineplot(
        data=grupo,
        x="Num_processos",
        y="Tempo_segundos",
        marker="o"
    )
    plt.title(f"Tempo de Execução - M={M}, N={N}")
    plt.xlabel("Número de Processos")
    plt.ylabel("Tempo (s)")
    plt.xticks(grupo["Num_processos"].unique())
    plt.grid(True)
    nome_arquivo = f"{output_dir}/tempo_M{M}_N{N}.png"
    plt.savefig(nome_arquivo)
    plt.close()

print(f"Gráficos salvos na pasta '{output_dir}'.")
