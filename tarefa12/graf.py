import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os

sns.set(style="whitegrid")

# Criar diretório para os gráficos, se não existir
output_dir = "graficos"
os.makedirs(output_dir, exist_ok=True)

# Carregar os dados
df_forte = pd.read_csv("resultados_forte.csv")
df_fraca = pd.read_csv("resultados_fraca_.csv")

# Função para plotar e salvar os gráficos
def plot_tempo(df, titulo, nome_arquivo):
    plt.figure(figsize=(12, 8))
    
    # Organizar a ordem das linhas
    df = df.sort_values(by=["n_threads" if "n_threads" in df.columns else "threads"])
    x_col = "n_threads" if "n_threads" in df.columns else "threads"

    for collapse in sorted(df["collapse"].unique()):
        for sched in ["static", "dynamic", "guided"]:
            subset = df[(df["schedule"] == sched) & (df["collapse"] == collapse)]
            if not subset.empty:
                plt.plot(subset[x_col], subset["tempo"], marker='o', label=f"{sched}, collapse={collapse}")

    plt.title(titulo)
    plt.xlabel("Número de Threads")
    plt.ylabel("Tempo de Execução (s)")
    plt.legend()
    plt.tight_layout()
    
    # Caminho completo do arquivo
    caminho_completo = os.path.join(output_dir, nome_arquivo)
    plt.savefig(caminho_completo)
    plt.show()

# Gerar e salvar gráficos
plot_tempo(df_forte, "Escalabilidade Forte: Tempo vs Threads", "escalabilidade_forte.png")
plot_tempo(df_fraca, "Escalabilidade Fraca: Tempo vs Threads", "escalabilidade_fraca.png")
