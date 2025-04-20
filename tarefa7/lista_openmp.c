#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Estrutura de nó
typedef struct Node {
    char filename[100];
    struct Node* next;
} Node;

// Função para criar um novo nó
Node* createNode(const char* filename) {
    Node* newNode = (Node*) malloc(sizeof(Node));
    snprintf(newNode->filename, sizeof(newNode->filename), "%s", filename);
    newNode->next = NULL;
    return newNode;
}

// Função para adicionar nó no final
void appendNode(Node** head, const char* filename) {
    Node* newNode = createNode(filename);
    if (*head == NULL) {
        *head = newNode;
        return;
    }
    Node* current = *head;
    while (current->next != NULL)
        current = current->next;
    current->next = newNode;
}

// Função para liberar memória
void freeList(Node* head) {
    Node* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

int main() {
    Node* head = NULL;

    // Criando lista com arquivos fictícios
    appendNode(&head, "arquivo1.txt");
    appendNode(&head, "arquivo2.txt");
    appendNode(&head, "arquivo3.txt");
    appendNode(&head, "arquivo4.txt");

    // Região paralela com tasks
    #pragma omp parallel
    {
        #pragma omp single // apenas uma thread cria as tasks
        {
            Node* current = head;
            while (current != NULL) {
                Node* taskNode = current;
                #pragma omp task
                {
                    int thread_id = omp_get_thread_num();
                    printf("Arquivo: %s | Thread: %d\n", taskNode->filename, thread_id);
                }
                current = current->next;
            }
        }
    }

    freeList(head);
    return 0;
}

// Esse código, como está, processa cada nó uma única vez e não repete.
// ✔️ O comportamento muda entre execuções pela natureza dinâmica de agendamento de tarefas do OpenMP.
// ✔️ A chave para garantir exclusividade é o #pragma omp single controlando a criação das tasks, e cada task guardando sua própria referência ao nó antes de ser criada.

// Se quiser, posso montar um experimento para medir quantas vezes cada arquivo é processado e simular situações mais complexas — só dizer! Quer? 🚀

// Terminal:
// gcc -fopenmp -o lista_openmp lista_openmp.c
// export OMP_NUM_THREADS=4
// ./lista_openmp