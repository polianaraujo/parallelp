import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os

# Configurações visuais
sns.set(style="whitegrid")
plt.rcParams["figure.dpi"] = 150

# Criar diretório para salvar o gráfico
output_dir = "graf_afinidade"
os.makedirs(output_dir, exist_ok=True)

# Carregar os dados
df = pd.read_csv("results_20250518_160541.csv")
df["threads"] = df["threads"].astype(int)

# Gráfico 1: Tempo vs Threads (uma linha por afinidade)
plt.figure(figsize=(10, 6))
sns.lineplot(data=df, x="threads", y="time", hue="affinity", marker="o")
plt.title("Tempo de Execução vs Número de Threads")
plt.xlabel("Número de Threads")
plt.ylabel("Tempo (s)")
plt.legend(title="Afinidade")
plt.tight_layout()
plt.savefig(f"{output_dir}/tempo_vs_threads_por_afinidade.png")
plt.close()

# Gráfico 2 único: Comparação de afinidades por número de threads
plt.figure(figsize=(12, 6))
sns.barplot(data=df, x="threads", y="time", hue="affinity", palette="Set2")
plt.title("Comparação de Afinidades para Diferentes Números de Threads")
plt.xlabel("Número de Threads")
plt.ylabel("Tempo de Execução (s)")
plt.legend(title="Afinidade", bbox_to_anchor=(1.05, 1))
plt.tight_layout()
plt.savefig(f"{output_dir}/comparacao_afinidades_todas_threads.png")
plt.close()

print(f"Gráfico salvo em: {output_dir}/comparacao_afinidades_todas_threads.png")
