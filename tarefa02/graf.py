import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import sys

def generate_graph(csv_filename='results.csv'):
    """
    Lê os dados de um arquivo CSV e gera um gráfico de barras para
    comparar o desempenho.
    """
    # Tenta ler o arquivo CSV. Se não existir, exibe uma mensagem de erro.
    try:
        data = pd.read_csv(csv_filename)
    except FileNotFoundError:
        print(f"Erro: O arquivo '{csv_filename}' não foi encontrado.")
        print("Certifique-se de que o nome do arquivo está correto e no mesmo diretório,")
        print("ou execute o script de compilação para gerá-lo.")
        sys.exit(1) # Encerra o script se o arquivo não for encontrado

    # Define um estilo visual mais agradável para o gráfico
    sns.set_theme(style="whitegrid")

    # Cria a figura e os eixos para o gráfico com um tamanho específico
    plt.figure(figsize=(14, 8))

    # Cria o gráfico de barras usando seaborn
    # x='Function': As categorias no eixo X (as 3 funções testadas).
    # y='ExecutionTime_s': Os valores no eixo Y (o tempo de execução).
    # hue='Optimization': A variável que agrupará as barras por cor (O0, O2, O3).
    barplot = sns.barplot(
        data=data,
        x='Function',
        y='ExecutionTime_s',
        hue='Optimization',
        palette='viridis' # Define uma paleta de cores
    )

    # Adiciona os valores exatos no topo de cada barra para facilitar a leitura
    for p in barplot.patches:
        barplot.annotate(format(p.get_height(), '.4f'),
                         (p.get_x() + p.get_width() / 2., p.get_height()),
                         ha='center', va='center',
                         xytext=(0, 9),
                         textcoords='offset points',
                         fontsize=10)

    # Configura os títulos e rótulos do gráfico para maior clareza
    plt.title('Impacto da Otimização do Compilador no Desempenho (ILP)', fontsize=16)
    plt.xlabel('Função Testada', fontsize=12)
    plt.ylabel('Tempo de Execução (em segundos)', fontsize=12)
    
    # Ajusta o layout para garantir que nada seja cortado
    plt.tight_layout()

    # Salva o gráfico como uma imagem PNG
    output_filename = 'grafico_desempenho.png'
    plt.savefig(output_filename)
    print(f"Gráfico gerado e salvo como '{output_filename}'")

    # Exibe o gráfico em uma janela
    plt.show()


if __name__ == '__main__':
    generate_graph()