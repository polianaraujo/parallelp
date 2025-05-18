import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os

# Configurações gerais
sns.set(style="whitegrid")

# Carregar os dados
file_path = "results_escalabilidade_forte.csv"
data = pd.read_csv(file_path)

# Converter valores de 'nan' para NaN
data.replace("nan", pd.NA, inplace=True)
data.dropna(inplace=True)

# Criar diretório para gráficos
os.makedirs("graficos_navier_stokes", exist_ok=True)

# Converter colunas numéricas
numeric_cols = ['ExecutionTime', 'CenterValue', 'AverageValue']
for col in numeric_cols:
    data[col] = pd.to_numeric(data[col], errors='coerce')

# 1. Análise de Tempo de Execução por Afinidade e Número de Threads
def plot_execution_time_by_affinity(data):
    plt.figure(figsize=(12, 6))
    sns.barplot(x="Affinity", y="ExecutionTime", hue="Threads", data=data)
    plt.title("Tempo de Execução por Afinidade e Número de Threads")
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.savefig("graficos_navier_stokes/execution_time_by_affinity.png")
    plt.show()

# 4. Escalabilidade Forte – Análise de Speedup
def calculate_speedup(data):
    baseline = data[data['Threads'] == 1]['ExecutionTime'].mean()
    data = data.copy()
    data['Speedup'] = baseline / data['ExecutionTime']
    return data


def plot_speedup_by_affinity(data):
    plt.figure(figsize=(12, 6))
    sns.lineplot(x="Threads", y="Speedup", hue="Affinity", data=data, marker="o")
    plt.title("Speedup por Afinidade e Número de Threads")
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.savefig("graficos_navier_stokes/speedup_by_affinity.png")
    plt.show()

# 5. Análise de Eficiência
def plot_efficiency_by_affinity(data):
    data['Efficiency'] = data['Speedup'] / data['Threads']
    plt.figure(figsize=(12, 6))
    sns.lineplot(x="Threads", y="Efficiency", hue="Affinity", data=data, marker="o")
    plt.title("Eficiência por Afinidade e Número de Threads")
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.savefig("graficos_navier_stokes/efficiency_by_affinity.png")
    plt.show()



def main():
    data_with_speedup = calculate_speedup(data)
    plot_execution_time_by_affinity(data_with_speedup)
    plot_speedup_by_affinity(data_with_speedup)
    plot_efficiency_by_affinity(data_with_speedup)


if __name__ == "__main__":
    main()
