# Relatório 15 - Difusão de calor usando MPI
Aluna: Poliana Ellen de Araújo

## 1. Introdução


## 2. Metodologia

### 2.1. Versão MPI_Send / MPI_Recv `heat_diffusion_denrecv.c`

- Comunicação bloqueante: processo envia e espera receber dados antes de continuar a computação.
- Troca de bordas primeiro, só depois atualiza o domínio.

🔁 Comunicação
```
MPI_Send(...);
MPI_Recv(...);
```

📌 Ordem das chamadas
1. Envia para vizinhos.
2. Recebe dos vizinhos.
3. Após recebimento, atualiza toda a região.

⚠️ Ponto de atenção
- Sincronização estrita: cada processo só avança quando os dados chegam.
- Não há sobreposição entre comunicação e computação.

⏱️ Desempenho
- Pode ser mais lento, especialmente em sistemas com latência alta.

### 2.2. Versão MPI_Isend / MPI_Irecv + MPI_Wait `heat_diffusion_isend_wait.c`

- Comunicação não bloqueante (MPI_Irecv, MPI_Isend) permite iniciar comunicações e só esperar no final.
- Mas ainda espera por todas antes de computar.

🔁 Comunicação
```
MPI_Irecv(...);  // iniciar recepção
MPI_Isend(...);  // iniciar envio
MPI_Waitall(...); // espera tudo terminar
```

🧠 Vantagem
A comunicação é assíncrona, e isso abre caminho para sobreposição, mas ela ainda não é explorada aqui.

⚠️ Ponto de atenção
- Espera por todas as mensagens antes de atualizar os valores.
- Nenhuma computação é feita antes do MPI_Waitall.

⏱️ Desempenho
Geralmente melhor que Send/Recv, mas ainda não ótimo.

### 2.3. Versão MPI_Test `heat_diffusion_overlap.c`

- Comunicação iniciada com MPI_Irecv e MPI_Isend.
- Enquanto espera os dados das bordas, já computa os pontos internos.
- Usa MPI_Test para checar se os dados chegaram. Se não chegaram, usa MPI_Wait.

```
MPI_Irecv(...);
MPI_Isend(...);
...
// Computa pontos internos enquanto espera comunicação
MPI_Test(...); // checa se dados chegaram
MPI_Wait(...); // se necessário, espera dados
```

🧠 Vantagem
- Sobreposição real de comunicação e computação:
    - Enquanto a mensagem não chega, o processo avança no cálculo.
    - Isso esconde a latência da comunicação.

⏱️ Desempenho
- Tendência a ser a mais rápida entre as três.
- Ideal quando o custo de comunicação e computação são comparáveis.

## 3. Resultados

|Versão|Comunicação|Computação sobreposta?|Tempo|Desempenho|
|------|-----------|----------------------|-----|----------|
|``MPI_Send`` / ``MPI_Recv``|Bloqueante|❌ Não|0.329468 s|🚫 Mais lento|
|``MPI_Isend`` / ``MPI_Irecv`` + ``Wait``|Não bloqueante|❌ Não|0.001555 s|⚠️ Intermediário|
|``MPI_Test`` + sobreposição|Não bloqueante|❌ Não|0.001590 s|✅ Melhor desempenho|


## 4. Conclusões

- A versão MPI_Send/Recv é mais simples, mas bloqueia o processo enquanto espera os dados. É adequada para testes ou casos onde comunicação não é gargalo.
- A versão MPI_Isend/Irecv + Wait melhora a escalabilidade e prepara o terreno para computação sobreposta, mas ainda espera todos os dados antes de computar.
- A versão MPI_Test permite esconder o custo de comunicação com computação útil, otimizando o uso dos recursos do processador.