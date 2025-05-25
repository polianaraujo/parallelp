# Metodologia

Para executar o código `heat.c` com offloading para GPU via OpenMP no cluster do NPAD/UFRN, é essencial considerar as configurações específicas do ambiente, incluindo os módulos disponíveis, compiladores suportados e políticas de agendamento de jobs. Abaixo, apresento orientações detalhadas para adaptar o código e o script de submissão, visando otimizar a execução no NPAD.

---

### 🔧 Adaptação do Código `heat.c` para Offloading com OpenMP

Para aproveitar a GPU eficientemente, é necessário:

1. **Movimentar os dados para a GPU** antes do loop principal.
2. **Executar o kernel `solve` na GPU** utilizando diretivas OpenMP.
3. **Atualizar os dados da GPU para a CPU** após o loop.
4. **Liberar os dados da GPU** ao final da execução.

#### 1. Inclusão de Headers Necessários

Adicione no início do código:

```c
#include <omp.h>
```

#### 2. Movimentação Inicial de Dados para a GPU

Após a inicialização das matrizes `u` e `u_tmp`, adicione:

```c
#pragma omp target enter data map(to: u[0:n*n], u_tmp[0:n*n])
```

#### 3. Modificação do Loop Principal para Offloading

Substitua o loop principal por:

```c
for (int t = 0; t < nsteps; ++t) {
    #pragma omp target teams distribute parallel for collapse(2)
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            const double r = alpha * dt / (dx * dx);
            const double r2 = 1.0 - 4.0*r;

            u_tmp[i+j*n] =  r2 * u[i+j*n] +
            r * ((i < n-1) ? u[i+1+j*n] : 0.0) +
            r * ((i > 0)   ? u[i-1+j*n] : 0.0) +
            r * ((j < n-1) ? u[i+(j+1)*n] : 0.0) +
            r * ((j > 0)   ? u[i+(j-1)*n] : 0.0);
        }
    }

    // Swap dos ponteiros na CPU
    double *tmp = u;
    u = u_tmp;
    u_tmp = tmp;
}
```

#### 4. Atualização dos Dados da GPU para a CPU

Antes de calcular o `l2norm`, adicione:

```c
#pragma omp target update from(u[0:n*n])
```

#### 5. Liberação dos Dados da GPU

Ao final do `main`, antes de `free(u)` e `free(u_tmp)`, adicione:

```c
#pragma omp target exit data map(delete: u[0:n*n], u_tmp[0:n*n])
```

---

### 📝 Script de Submissão `job_submit.sh` para o NPAD/UFRN

O NPAD utiliza o gerenciador de filas SLURM. Abaixo, um exemplo de script adaptado:

```bash
#!/bin/bash
#SBATCH --job-name=heat_gpu
#SBATCH --partition=gpu-8-v100
#SBATCH --gres=gpu:1
#SBATCH --cpus-per-task=4
#SBATCH --time=03:00:00
#SBATCH --output=heat_output_%j.txt

# Carregar módulos necessários
module load gcc/11.2.0
module load openmpi
module load cuda

# Variáveis de ambiente para OpenMP
export OMP_TARGET_OFFLOAD=MANDATORY
export OMP_NUM_THREADS=4

# Compilação
make

# Execução
./heat 1000 100
```

**Notas:**

* **`--partition=gpu-8-v100`**: Especifica a partição com GPUs V100.
* **`--gres=gpu:1`**: Solicita uma GPU.
* **`--cpus-per-task=4`**: Aloca 4 CPUs para a tarefa.
* **`--time=03:00:00`**: Define o tempo máximo de execução como 3 horas.
* **`--output=heat_output_%j.txt`**: Redireciona a saída para um arquivo com o ID do job.

---

### 🛠️ Compilação com Suporte a OpenMP Offloading

Certifique-se de que o compilador utilizado suporta OpenMP com offloading para GPU. No caso do GCC, a partir da versão 9, há suporte experimental para offloading para GPUs NVIDIA. A compilação pode ser realizada com:

```bash
gcc -fopenmp -foffload=nvptx-none -O3 -o heat heat.c -lm
```

**Notas:**

* **`-fopenmp`**: Habilita o suporte a OpenMP.
* **`-foffload=nvptx-none`**: Especifica o alvo de offloading para GPUs NVIDIA.
* **`-O3`**: Nível de otimização.
* **`-lm`**: Linka a biblioteca matemática.

---

### 📌 Considerações Finais

* **Validação**: Após a execução, verifique os resultados e o tempo de execução para garantir que o offloading está ocorrendo corretamente.
* **Monitoramento**: Utilize ferramentas como `nvidia-smi` para monitorar a utilização da GPU durante a execução.
* **Ajustes**: Dependendo do desempenho observado, ajuste os parâmetros de compilação e as diretivas OpenMP para otimizar a execução.

Caso necessite de assistência adicional ou tenha dúvidas específicas sobre o ambiente do NPAD/UFRN, recomendo entrar em contato com a equipe de suporte técnico do NPAD através do e-mail [atendimento@npad.ufrn.br](mailto:atendimento@npad.ufrn.br) ou consultar os repositórios disponíveis no GitHub do NPAD: [https://github.com/NPAD-UFRN](https://github.com/NPAD-UFRN).
