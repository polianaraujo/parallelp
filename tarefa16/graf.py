import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os

# Configurações estéticas
sns.set(style="whitegrid")
plt.rcParams.update({'figure.max_open_warning': 0})

# Carrega os dados
df = pd.read_csv("resultados.csv")

# Verifica os dados carregados
print(df.head())

# Criar diretório para salvar o gráfico
output_dir = "images"
os.makedirs(output_dir, exist_ok=True)

# Gráfico 1: Tempo vs Número de Processos (para cada combinação de M e N)
for (m, n), group in df.groupby(["M", "N"]):
    plt.figure(figsize=(8, 5))
    sns.lineplot(x="Num_processos", y="Tempo_segundos", data=group, marker="o")
    plt.title(f"Tempo de Execução vs Nº de Processos (M={m}, N={n})")
    plt.xlabel("Número de Processos")
    plt.ylabel("Tempo (segundos)")
    plt.xticks(group["Num_processos"].unique())
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(f"images/tempo_vs_processos_M{m}_N{n}.png")
    plt.close()

# Gráfico 2: Tempo vs M (fixando N e Num_processos)
for (n, p), group in df.groupby(["N", "Num_processos"]):
    plt.figure(figsize=(8, 5))
    sns.lineplot(x="M", y="Tempo_segundos", data=group, marker="o")
    plt.title(f"Tempo vs M (N={n}, Processos={p})")
    plt.xlabel("Número de Linhas da Matriz (M)")
    plt.ylabel("Tempo (segundos)")
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(f"images/tempo_vs_M_N{n}_P{p}.png")
    plt.close()

# Gráfico 3: Tempo vs N (fixando M e Num_processos)
for (m, p), group in df.groupby(["M", "Num_processos"]):
    plt.figure(figsize=(8, 5))
    sns.lineplot(x="N", y="Tempo_segundos", data=group, marker="o")
    plt.title(f"Tempo vs N (M={m}, Processos={p})")
    plt.xlabel("Número de Colunas da Matriz / Tamanho de x (N)")
    plt.ylabel("Tempo (segundos)")
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(f"images/tempo_vs_N_M{m}_P{p}.png")
    plt.close()

print("✅ Gráficos salvos como arquivos PNG!")
