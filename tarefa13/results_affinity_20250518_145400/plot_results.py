import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('affinity_results.csv')
pivot_df = df.pivot(index='num_threads', columns='affinity_type', values='execution_time')

plt.figure(figsize=(10,6))
for column in pivot_df.columns:
    plt.plot(pivot_df.index, pivot_df[column], marker='o', label=column)

plt.title('Escalabilidade do Código de Navier-Stokes')
plt.xlabel('Número de Threads')
plt.ylabel('Tempo de Execução (s)')
plt.legend(title='Afinidade')
plt.grid(True)
plt.savefig('affinity_scaling.png')
plt.close()
