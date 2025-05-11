#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

typedef struct node {
    char filename[100];
    struct node* next;
} Node;

// Função para adicionar um novo nó ao final da lista
void append(Node** head_ref, const char* filename) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    strcpy(new_node->filename, filename);
    new_node->next = NULL;

    if (*head_ref == NULL) {
        *head_ref = new_node;
        return;
    }

    Node* temp = *head_ref;
    while (temp->next != NULL)
        temp = temp->next;

    temp->next = new_node;
}

// Função para liberar a memória da lista
void free_list(Node* head) {
    while (head != NULL) {
        Node* temp = head;
        head = head->next;
        free(temp);
    }
}

int main() {
    Node* head = NULL;

    // Criando lista com arquivos fictícios
    append(&head, "arquivo1.txt");
    append(&head, "arquivo2.txt");
    append(&head, "arquivo3.txt");
    append(&head, "arquivo4.txt");
    append(&head, "arquivo5.txt");

    Node* current = head;

    #pragma omp parallel
    {
        #pragma omp single
        {
            while (current != NULL) {
                Node* temp = current;
                current = current->next;

                #pragma omp task firstprivate(temp)
                {
                    printf("Arquivo: %s, Thread: %d\n", temp->filename, omp_get_thread_num());
                }
            }
        }
    }

    free_list(head);

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