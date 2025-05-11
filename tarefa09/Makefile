# Compilador e flags
CC = gcc
CFLAGS = -fopenmp -Wall

# Alvos (executáveis)
TARGETS = code1 code2

# Regra padrão: compila todos os alvos
all: $(TARGETS)

# Compila code1.c para gerar o executável code1
code1: code1.c
	$(CC) $(CFLAGS) code1.c -o code1

# Compila code2.c para gerar o executável code2
code2: code2.c
	$(CC) $(CFLAGS) code2.c -o code2

# Limpeza dos arquivos executáveis
clean:
	rm -f $(TARGETS)
