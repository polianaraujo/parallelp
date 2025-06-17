import pandas as pd
import matplotlib.pyplot as plt

# Nome do arquivo CSV gerado pelo programa C
csv_filename = 'resultados.csv'

# Tenta ler o arquivo CSV
try:
    data = pd.read_csv(csv_filename)
except FileNotFoundError:
    print(f"Erro: Arquivo '{csv_filename}' não encontrado.")
    print("Por favor, execute o programa C primeiro para gerar o arquivo.")
    exit()

# Configura o gráfico
plt.figure(figsize=(12, 8))

# Plota os dados de cada método
plt.plot(data['Tamanho_N'], data['Tempo_Linha_us'], marker='o', linestyle='-', label='Linha por Linha (Cache-Friendly)')
plt.plot(data['Tamanho_N'], data['Tempo_Coluna_us'], marker='x', linestyle='--', label='Coluna por Coluna (Cache-Unfriendly)')

# Adiciona títulos e rótulos
plt.title('Comparação de Desempenho: Multiplicação Matriz-Vetor', fontsize=16)
plt.xlabel('Tamanho da Matriz (N x N)', fontsize=12)
plt.ylabel('Tempo de Execução (microssegundos) - Escala Logarítmica', fontsize=12)

# Usa escala logarítmica no eixo Y para melhor visualização da divergência
# plt.yscale('log')

# Adiciona legenda e grade
plt.legend(fontsize=12)
plt.grid(True, which='both', linestyle='--', linewidth=0.5)

# Salva o gráfico em um arquivo de imagem
output_image_filename = 'grafico_desempenho.png'
plt.savefig(output_image_filename)

print(f"Gráfico salvo com sucesso como '{output_image_filename}'")

# Opcional: mostra o gráfico na tela
# plt.show()