#include "../include/queue.h"

Queue* createQueue() {
    Queue *q = (Queue*)malloc(sizeof(Queue));
    if (q == NULL) return NULL;
    q->head = q->tail = NULL;
    q->size = 0;
    return q;
}
void enqueue(Queue* q, PCB* p) {
    if (q == NULL || p == NULL) return;

    Node *newNode = (Node*)malloc(sizeof(Node));
    newNode->process = p;
    newNode->next = NULL;

    if (q->tail == NULL) {
        q->head = q->tail = newNode;
    } else {
        q->tail->next = newNode;
        q->tail = newNode;
    }
    q->size++;
}
PCB* dequeue(Queue* q) {
    if (q == NULL || q->head == NULL) return NULL;

    Node *temp = q->head;
    PCB *p = temp->process;

    q->head = q->head->next;
    
    if (q->head == NULL) {
        q->tail = NULL;
    }

    free(temp);
    q->size--;
    return p;
}
bool is_empty(Queue* q) {
   return (q == NULL || q->head == NULL);
}
void print_queue(Queue* q, const char* queue_name) {
    printf("%s: ", queue_name);
    
    if (isempty(q)) {
        printf("Empty\n");
        return;
    }

    Node* current = q->head;
    while (current != NULL) {
        // Printing the Process ID as required [cite: 53, 124]
        printf("P%d", current->process->processID);
        
        current = current->next;
        if (current != NULL) {
            printf(" -> ");
        }
    }
    printf("\n");
}