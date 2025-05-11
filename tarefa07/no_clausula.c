// sem cláusulas (tipo single, taskwait ou task) o comportamento vai mudar bastante porque ficamos limitados ao #pragma omp parallel puro — e isso significa que todas as threads executam o mesmo código.

// Vou fazer o código assim, sem single, task ou taskwait, apenas com #pragma omp parallel. Isso vai te ajudar a ver o que acontece:


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
    char filename[100];
    struct node* next;
} Node;

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

void free_list(Node* head) {
    while (head != NULL) {
        Node* temp = head;
        head = head->next;
        free(temp);
    }
}

int main() {
    Node* head = NULL;

    append(&head, "arquivo1.txt");
    append(&head, "arquivo2.txt");
    append(&head, "arquivo3.txt");
    append(&head, "arquivo4.txt");
    append(&head, "arquivo5.txt");

    Node* current = head;

    // Versão sequencial
    while (current != NULL) {
        printf("Arquivo: %s\n", current->filename);
        current = current->next;
    }

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