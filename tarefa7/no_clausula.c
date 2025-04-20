// sem cláusulas (tipo single, taskwait ou task) o comportamento vai mudar bastante porque ficamos limitados ao #pragma omp parallel puro — e isso significa que todas as threads executam o mesmo código.

// Vou fazer o código assim, sem single, task ou taskwait, apenas com #pragma omp parallel. Isso vai te ajudar a ver o que acontece:


#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Definição do nó da lista
// Assim, é possível fazer uma lista encadeada onde cada nó aponta para o próximo
typedef struct Node {
    char filename[100]; // guardar o nome do arquivo (string)
    struct Node* next;  // um ponteiro para o próximo nó da lista
} Node;

// Função para criar um novo nó
// - Aloca memória para um novo nó (malloc)
// - Copia o nome do arquivo para 'filename'
// - Deixa 'next' como NULL (porque inicialmente ele não aponta para nada)
// - Retorna o ponteiro para o novo nó
Node* create_node(const char* name) {
    Node* new_node = (Node*) malloc(sizeof(Node));
    snprintf(new_node->filename, 100, "%s", name);
    new_node->next = NULL;
    return new_node;
}

// Função para liberar a lista
// - Percorre a lista
// - Guarda o ponteiro atual em 'temp'
// - Move 'head' para o próximo nó
// - Libera a memória (free(temp))
// Assim a gente evita vazamento de memória no final do programa
void free_list(Node* head) {
    Node* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

int main() {
    // Criando a lista encadeada
    Node* head = create_node("arquivo1.txt");       // head : aponta para o primeiro nó
    head->next = create_node("arquivo2.txt");       // head : aponto para o segundo nó
    head->next->next = create_node("arquivo3.txt"); // head : aponto para o terceiro nó...
    head->next->next->next = create_node("arquivo4.txt");
    head->next->next->next->next = create_node("arquivo5.txt");

    Node* current;

    // Região paralela SEM cláusulas
    #pragma omp parallel    // Cria várias threads
    {
        // Cada thread pega seu 'thread_id' com 'omp_get_thread_num()'
        int thread_id = omp_get_thread_num();
        current = head;

        // Todas as threads percorrem a lista inteira e imprimem o nome do arquivo com o id da thread

        while (current != NULL) {
            printf("Arquivo: %s | Thread: %d\n", current->filename, thread_id);
            current = current->next;
        }
    }

    // Liberando memória (lliberando a lista)
    //Liberamos a memória alocada para lista encadeada
    free_list(head);

    return 0;
}

// Como não tem nenhuma cláusuça (single, for, task ...), todas as threads fazem o mesmo: cada uma percorre a lista inteira e imprime todos os arquivos.

// Todas as threads vão executar o while inteiro
// ➝ Ou seja, cada thread percorre a lista inteira e imprime todos os arquivos, cada vez com o id da sua thread.

// Todos os nós são processados?
// ➝ Sim, por todas as threads. Como não há divisão de tarefas, cada thread executa tudo.

// Algum nó foi processado mais de uma vez ou ignorado?
// ➝ Sim — cada arquivo é processado por todas as threads, ou seja, impresso várias vezes (uma vez por thread).

// O comportamento muda entre execuções?
// ➝ Pode mudar a ordem das threads imprimindo, mas todas as threads vão percorrer a lista inteira sempre, pois todas entram no #pragma omp parallel e fazem o mesmo while.

// ✅ Conclusão
// Sem as cláusulas single, task e taskwait, não há controle de quem faz o quê — todas as threads entram no código e executam tudo.
// Se quiser processar de forma paralela controlada, precisa organizar com single+task ou usar for com #pragma omp for para dividir bem.


// Terminal:
// gcc -fopenmp no_clausula.c -o no_clausula
// export OMP_NUM_THREADS=4
// ./no_clausula