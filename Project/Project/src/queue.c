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
   return (q == NULL || q->head == NULL || q->size == 0);
}

void print_queue(Queue* q, const char* queue_name) {
    printf("%s: ", queue_name);
    
    if (is_empty(q)) {
        printf("Empty\n");
        return;
    }

    Node* current = q->head;
    while (current != NULL) {
        
        printf("P%d", current->process->pid);
        
        current = current->next;
        if (current != NULL) {
            printf(" -> ");
        }
    }
    printf("\n");
}
PCB* find_and_remove_best_hrrn(Queue* q) {
    if (is_empty(q)) return NULL;

    Node* curr = q->head;
    Node* prev = NULL;
    Node* best_node = q->head;
    Node* best_prev = NULL;
    double max_ratio = -1.0;

    
    while (curr != NULL) {
        
        double b_time = (curr->process->burst_time <= 0) ? 1.0 : (double)curr->process->burst_time;
        double ratio = (double)(curr->process->waiting_time + b_time) / b_time;
        if (ratio > max_ratio) {
            max_ratio = ratio;
            best_node = curr;
            best_prev = prev;
        }
        prev = curr;
        curr = curr->next;
    }

    
    if (best_prev == NULL) q->head = best_node->next;
    else best_prev->next = best_node->next;

    if (best_node == q->tail) q->tail = best_prev;

    PCB* selected_pcb = best_node->process;
    free(best_node);
    q->size--;
    
    return selected_pcb;
}

void remove_from_queue(Queue** q_ptr, PCB* process_to_remove) {
    
    if (q_ptr == NULL || *q_ptr == NULL) return;
    Queue* q = *q_ptr;

    if (q == NULL || q->head == NULL || process_to_remove == NULL) {
        return;
    }

    Node* curr = q->head;
    Node* prev = NULL;

    
    while (curr != NULL && curr->process != process_to_remove) {
        prev = curr;
        curr = curr->next;
    }

   
    if (curr == NULL) return;

    
    if (prev == NULL) {
        
        q->head = curr->next;
    } else {
       
        prev->next = curr->next;
    }

    
    if (curr == q->tail) {
        q->tail = prev;
    }

    
    free(curr);
    q->size--;
}
void freeQueue(Queue* q) {
    if (q == NULL) return;
    Node* curr = q->head;
    while (curr != NULL) {
        Node* next = curr->next;
        
        free(curr);
        curr = next;
    }
    free(q);
}