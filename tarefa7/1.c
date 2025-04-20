#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

// Estrutura de nó para a lista encadeada
typedef struct Node {
    char filename[100];
    struct Node* next;
} Node;

// Função para adicionar um nó no final da lista
void append(Node** head, const char* filename) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    strcpy(new_node->filename, filename);
    new_node->next = NULL;

    if (*head == NULL) {
        *head = new_node;
    } else {
        Node* temp = *head;
        while (temp->next)
            temp = temp->next;
        temp->next = new_node;
    }
}

// Função para liberar a memória da lista
void free_list(Node* head) {
    Node* temp;
    while (head) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

int main() {
    Node* head = NULL;

    // Criando alguns arquivos fictícios
    append(&head, "arquivo1.txt");
    append(&head, "arquivo2.txt");
    append(&head, "arquivo3.txt");
    append(&head, "arquivo4.txt");

    Node* current = head;

    // Região paralela com tarefas
    #pragma omp parallel
    {
        #pragma omp single // Apenas uma thread cria as tarefas
        {
            while (current) {
                Node* node = current;
                current = current->next;

                #pragma omp task firstprivate(node)
                {
                    printf("Processando %s na thread %d\n", node->filename, omp_get_thread_num());
                }
            }
        }
    }

    free_list(head);
    return 0;
}


// gcc -fopenmp programa.c -o programa
// ./programa

// Reflexão sobre a Execução
// Todos os nós foram processados?
// Provavelmente sim, mas não é garantido em todos os ambientes se a sincronização correta não for feita. A criação das tarefas pode acontecer antes que elas sejam executadas.

// Algum foi processado mais de uma vez ou ignorado?
// Com o uso de firstprivate(node) na #pragma omp task, cada tarefa recebe sua própria cópia do ponteiro para o nó, o que evita conflitos e repetições.

// Se isso não for usado, pode haver condições de corrida, onde múltiplas tarefas usam o mesmo ponteiro.

// O comportamento muda entre execuções?
// Sim, porque tarefas são executadas por diferentes threads de forma não determinística.

// A ordem dos prints pode variar, assim como quais threads executam cada tarefa.

// ------------------------------------------------------

// Como garantir que cada nó seja processado apenas uma vez por uma única tarefa?
// As boas práticas incluem:

// #pragma omp single: Garante que apenas uma thread crie as tarefas.

// firstprivate(node): Garante que cada tarefa tenha uma cópia exclusiva do ponteiro para o nó.

// Evitar modificar a lista dentro das tarefas: Para evitar condições de corrida.

// Opcionalmente, para forçar a finalização de todas as tarefas antes de seguir adiante, pode-se usar:

// # pragma omp taskwait