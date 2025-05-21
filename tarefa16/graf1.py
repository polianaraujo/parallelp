import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os

# Configurações
sns.set(style="whitegrid")

# Lê os dados do CSV
df = pd.read_csv("resultados.csv")

# Criar diretório para salvar o gráfico
# output_dir = "images1"
# os.makedirs(output_dir, exist_ok=True)

# Cria uma nova coluna com o tamanho da matriz como rótulo (ex: "M=1000, N=500")
df["Tamanho_matriz"] = df.apply(lambda row: f"M={row['M']}, N={row['N']}", axis=1)

# Cria o gráfico
plt.figure(figsize=(10, 6))
sns.lineplot(data=df, x="Num_processos", y="Tempo_segundos", hue="Tamanho_matriz", marker="o")

# Personalização
plt.title("Tempo de Execução vs Número de Processos")
plt.xlabel("Número de Processos")
plt.ylabel("Tempo de Execução (segundos)")
plt.legend(title="Tamanho da Matriz")
plt.grid(True)
plt.tight_layout()

# Salva o gráfico
plt.savefig("tempo_vs_processos_tamanhos_matrizes.png")
# plt.show()
