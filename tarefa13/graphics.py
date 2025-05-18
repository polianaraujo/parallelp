import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os

# Estilo de gráfico
sns.set(style="whitegrid")

# Carregando os dados
df = pd.read_csv("results_escalabilidade_forte.csv", on_bad_lines='skip')

# Garantindo que valores numéricos estejam corretos
df['Time'] = pd.to_numeric(df['Time'], errors='coerce')
df['Threads'] = pd.to_numeric(df['Threads'], errors='coerce')
df['Schedule'] = df['Schedule'].astype(str)

# Criar diretório para gráficos
os.makedirs("graficos_navier_stokes", exist_ok=True)

# Gráfico 1: Tempo vs Threads por Schedule
plt.figure(figsize=(10, 6))
for schedule in df['Schedule'].unique():
    sub_df = df[df['Schedule'] == schedule]
    plt.plot(sub_df['Threads'], sub_df['Time'], marker='o', label=schedule)

plt.title("Tempo de Execução vs Threads por Tipo de Afinidade (Schedule)")
plt.xlabel("Número de Threads")
plt.ylabel("Tempo de Execução (s)")
plt.legend(title="Schedule")
plt.xticks(df['Threads'].unique())
plt.grid(True)
plt.tight_layout()
plt.savefig("graficos_navier_stokes/tempo_vs_threads_schedule.png")
plt.close()

# Gráfico 2: Speedup teórico (Thread 1 como base)
base_time = df[df['Threads'] == 1].groupby('Schedule')['Time'].mean()
speedup_df = df.copy()
speedup_df['Speedup'] = speedup_df.apply(lambda row: base_time.get(row['Schedule'], 1) / row['Time'], axis=1)

plt.figure(figsize=(10, 6))
for schedule in speedup_df['Schedule'].unique():
    sub_df = speedup_df[speedup_df['Schedule'] == schedule]
    plt.plot(sub_df['Threads'], sub_df['Speedup'], marker='o', label=schedule)

plt.title("Speedup vs Threads por Tipo de Afinidade (Schedule)")
plt.xlabel("Número de Threads")
plt.ylabel("Speedup")
plt.legend(title="Schedule")
plt.xticks(df['Threads'].unique())
plt.grid(True)
plt.tight_layout()
plt.savefig("graficos_navier_stokes/speedup_vs_threads_schedule.png")
plt.close()

# Gráfico 3: CenterValue vs Threads por Schedule (opcional)
plt.figure(figsize=(10, 6))
for schedule in df['Schedule'].unique():
    sub_df = df[df['Schedule'] == schedule]
    plt.plot(sub_df['Threads'], sub_df['CenterValue'], marker='o', label=schedule)

plt.title("Valor Central vs Threads por Schedule")
plt.xlabel("Número de Threads")
plt.ylabel("CenterValue")
plt.legend(title="Schedule")
plt.xticks(df['Threads'].unique())
plt.grid(True)
plt.tight_layout()
plt.savefig("graficos_navier_stokes/centervalue_vs_threads_schedule.png")
plt.close()
