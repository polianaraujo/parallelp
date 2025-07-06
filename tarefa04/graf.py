import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import sys
import subprocess

def check_install_packages():
    """Verifica se as bibliotecas necessárias estão instaladas e, se não, as instala."""
    try:
        import pandas
        import matplotlib
        import seaborn
    except ImportError:
        print("Bibliotecas necessárias (pandas, matplotlib, seaborn) não encontradas. Tentando instalar...")
        try:
            subprocess.check_call([sys.executable, "-m", "pip", "install", "pandas", "matplotlib", "seaborn"])
            print("Bibliotecas instaladas com sucesso. Por favor, execute o script novamente.")
            sys.exit(0)
        except Exception as e:
            print(f"Falha ao instalar as bibliotecas: {e}")
            sys.exit(1)

def plot_results():
    """
    Lê os dados de desempenho dos arquivos CSV e gera gráficos
    para analisar o tempo de execução e o speedup.
    """
    try:
        mem_df = pd.read_csv("memory_results.csv").sort_values(by='Threads').reset_index(drop=True)
        cpu_df = pd.read_csv("compute_results.csv").sort_values(by='Threads').reset_index(drop=True)
    except FileNotFoundError as e:
        print(f"Erro: Arquivo não encontrado -> {e.filename}")
        print("Por favor, execute o script 'run.sh' para gerar os dados primeiro.")
        return

    # Estilo do gráfico
    sns.set_theme(style="whitegrid")
    
    # --- GRÁFICO 1: TEMPO DE EXECUÇÃO ---
    plt.figure(figsize=(12, 7))
    plt.plot(mem_df['Threads'], mem_df['ExecutionTime_micros'] / 1_000_000, 'o-', label='Limitado por Memória', color='blue')
    plt.plot(cpu_df['Threads'], cpu_df['ExecutionTime_micros'] / 1_000_000, 's-', label='Limitado por CPU', color='red')
    
    plt.title('Tempo de Execução vs. Número de Threads', fontsize=16, weight='bold')
    plt.xlabel('Número de Threads', fontsize=12)
    plt.ylabel('Tempo de Execução (segundos)', fontsize=12)
    plt.xticks(mem_df['Threads'])
    plt.legend()
    plt.grid(True, which='both', linestyle='--', linewidth=0.5)
    plt.savefig("grafico_tempo_execucao.png")
    print("Gráfico 'grafico_tempo_execucao.png' gerado.")

    # --- GRÁFICO 2: ANÁLISE DE SPEEDUP ---
    # Speedup é T(1)/T(N)
    t1_mem = mem_df.loc[mem_df['Threads'] == 1, 'ExecutionTime_micros'].iloc[0]
    t1_cpu = cpu_df.loc[cpu_df['Threads'] == 1, 'ExecutionTime_micros'].iloc[0]
    
    mem_df['Speedup'] = t1_mem / mem_df['ExecutionTime_micros']
    cpu_df['Speedup'] = t1_cpu / cpu_df['ExecutionTime_micros']

    plt.figure(figsize=(12, 7))
    # Linha de speedup ideal (linear)
    plt.plot(mem_df['Threads'], mem_df['Threads'], '--', label='Speedup Ideal (Linear)', color='gray')
    
    # Speedup dos programas
    plt.plot(mem_df['Threads'], mem_df['Speedup'], 'o-', label='Speedup (Limitado por Memória)', color='blue')
    plt.plot(cpu_df['Threads'], cpu_df['Speedup'], 's-', label='Speedup (Limitado por CPU)', color='red')

    plt.title('Análise de Speedup vs. Número de Threads', fontsize=16, weight='bold')
    plt.xlabel('Número de Threads', fontsize=12)
    plt.ylabel('Speedup ($T_1 / T_N$)', fontsize=12)
    plt.xticks(mem_df['Threads'])
    plt.legend()
    plt.grid(True, which='both', linestyle='--', linewidth=0.5)
    plt.savefig("grafico_speedup.png")
    print("Gráfico 'grafico_speedup.png' gerado.")
    
    # plt.show()

if __name__ == '__main__':
    check_install_packages()
    plot_results()