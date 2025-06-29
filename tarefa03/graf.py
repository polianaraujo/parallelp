import pandas as pd
import matplotlib.pyplot as plt
import sys

# Nome do arquivo CSV a ser lido
CSV_FILE = 'resultados_pi.csv'

def criar_grafico():
    """
    Lê os dados de um arquivo CSV e gera os gráficos de análise.
    """
    try:
        # Lê o arquivo CSV para um DataFrame do pandas
        data = pd.read_csv(CSV_FILE)
    except FileNotFoundError:
        print(f"Erro: Arquivo '{CSV_FILE}' não encontrado.")
        print("Por favor, execute o programa em C primeiro para gerar o arquivo de dados.")
        sys.exit(1) # Encerra o script com um código de erro

    # Extrai os dados das colunas do DataFrame
    iteracoes = data['Iteracoes']
    aproximacao = data['Pi_Calculado']
    diferenca = data['Diferenca']
    tempo_gasto = data['Tempo_s']

    # --- Configuração dos Gráficos ---
    plt.style.use('seaborn-v0_8-whitegrid')
    fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(10, 12))
    fig.suptitle('Análise do Cálculo de π (Dados de CSV)', fontsize=16, y=0.95)

    # --- Gráfico 1: Acurácia vs. Iterações ---
    ax1.plot(iteracoes, diferenca, marker='o', linestyle='-', color='b')
    ax1.set_xscale('log')
    ax1.set_yscale('log')
    ax1.set_title('Acurácia vs. Número de Iterações')
    ax1.set_xlabel('Número de Iterações (escala logarítmica)')
    ax1.set_ylabel('Diferença para o π Real (escala logarítmica)')
    ax1.grid(True, which="both", ls="--")

    # --- Gráfico 2: Tempo de Execução vs. Iterações ---
    ax2.plot(iteracoes, tempo_gasto, marker='o', linestyle='-', color='r')
    ax2.set_xscale('log')
    ax2.set_yscale('log')
    ax2.set_title('Tempo de Execução vs. Número de Iterações')
    ax2.set_xlabel('Número de Iterações (escala logarítmica)')
    ax2.set_ylabel('Tempo de Execução em Segundos (escala logarítmica)')
    ax2.grid(True, which="both", ls="--")
    
    # --- Gráfico 3: Valor Aproximado vs. Iterações ---
    ax3.plot(iteracoes, aproximacao, marker='o', linestyle='-', color='r')
    ax3.set_xscale('log')
    # ax3.set_yscale('log')
    ax3.set_title('Tempo de Execução vs. Número de Iterações')
    ax3.set_xlabel('Número de Iterações (escala logarítmica)')
    ax3.set_ylabel('Valor de PI Aproximado')
    ax3.grid(True, which="both", ls="--")

    plt.tight_layout(rect=[0, 0, 1, 0.95])
    
    output_image = 'analise_pi_calculo.png'
    plt.savefig(output_image, dpi=300)
    print(f"Gráfico salvo como '{output_image}'")
    # plt.show() # Descomente esta linha se quiser que o gráfico apareça na tela

if __name__ == '__main__':
    criar_grafico()