//
// Created by Andrew Boun on 2/24/25.
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <priority_queue.h>

// Initialize a new priority queue
PriorityQueue* pq_create(
    int capacity,
    int key_range,
    void* (*copy)(const void*),
    void (*destroy)(void*)
) {
    PriorityQueue* pq = malloc(sizeof(PriorityQueue));
    if (!pq) return NULL;
    
    pq->heap = malloc(capacity * sizeof(PQNode));
    pq->index = malloc(key_range * sizeof(int));
    pq->rev_index = malloc(capacity * sizeof(int));
    
    if (!pq->heap || !pq->index || !pq->rev_index) {
        free(pq->heap);
        free(pq->index);
        free(pq->rev_index);
        free(pq);
        return NULL;
    }
    
    // Initialize index arrays to -1 (not present)
    for (int i = 0; i < key_range; i++) {
        pq->index[i] = -1;
    }
    
    pq->size = 0;
    pq->capacity = capacity;
    pq->key_range = key_range;
    pq->copy = copy;
    pq->destroy = destroy;
    
    return pq;
}

// Free all memory used by the priority queue
void pq_destroy(PriorityQueue* pq) {
    if (pq) {
        for (int i = 0; i < pq->size; i++) {
            if (pq->destroy) {
                pq->destroy(pq->heap[i].data);
            }
        }
        free(pq->heap);
        free(pq->index);
        free(pq->rev_index);
        free(pq);
    }
}

// Check if the priority queue is empty
int pq_is_empty(PriorityQueue* pq) {
    return pq->size == 0;
}

// Get the index of the parent node
static int parent(int i) {
    return (i - 1) / 2;
}

// Get the index of the left child
static int left_child(int i) {
    return 2 * i + 1;
}

// Get the index of the right child
static int right_child(int i) {
    return 2 * i + 2;
}

static void update_indices(PriorityQueue* pq, int heap_pos, int key) {
    pq->index[key] = heap_pos;
    pq->rev_index[heap_pos] = key;
}

// Swap two nodes in the heap
static void pq_swap(PriorityQueue* pq, int i, int j) {
    PQNode temp = pq->heap[i];
    pq->heap[i] = pq->heap[j];
    pq->heap[j] = temp;
    
    // Update index mappings
    update_indices(pq, i, pq->heap[i].key);
    update_indices(pq, j, pq->heap[j].key);
}

// Maintain heap property by moving a node upward
static void heapify_up(PriorityQueue* pq, int i) {
    while (i > 0 && pq->heap[i].priority < pq->heap[parent(i)].priority) {
        pq_swap(pq, i, parent(i));
        i = parent(i);
    }
}

// Maintain heap property by moving a node downward
static void heapify_down(PriorityQueue* pq, int i) {
    int smallest = i;
    int left = left_child(i);
    int right = right_child(i);
    
    if (left < pq->size && pq->heap[left].priority < pq->heap[smallest].priority) {
        smallest = left;
    }
    
    if (right < pq->size && pq->heap[right].priority < pq->heap[smallest].priority) {
        smallest = right;
    }
    
    if (smallest != i) {
        pq_swap(pq, i, smallest);
        heapify_down(pq, smallest);
    }
}

// Insert a data with its priority into the priority queue
void pq_insert(PriorityQueue* pq, int key, void* data, int priority) {
    if (pq->size >= pq->capacity || key >= pq->key_range || pq->index[key] != -1) {
        return;
    }
    
    int i = pq->size++;
    pq->heap[i].key = key;
    pq->heap[i].data = pq->copy ? pq->copy(data) : data;
    pq->heap[i].priority = priority;
    
    update_indices(pq, i, key);
    heapify_up(pq, i);
}

// Extract the data with minimum priority
void* pq_extract_min(PriorityQueue* pq) {
    if (pq_is_empty(pq)) return NULL;
    
    void* min_data = pq->heap[0].data;
    int min_key = pq->heap[0].key;
    
    pq->size--;
    if (pq->size > 0) {
        pq->heap[0] = pq->heap[pq->size];
        update_indices(pq, 0, pq->heap[0].key);
        heapify_down(pq, 0);
    }
    
    pq->index[min_key] = -1;
    return min_data;
}

// Update the priority of a data in the queue
void pq_update_priority(PriorityQueue* pq, int key, int new_priority) {
    int pos = pq->index[key];
    if (pos == -1) return;
    
    int old_priority = pq->heap[pos].priority;
    pq->heap[pos].priority = new_priority;
    
    if (new_priority < old_priority)
        heapify_up(pq, pos);
    else
        heapify_down(pq, pos);
}

// Get the current priority of a data in the queue
int pq_get_priority(PriorityQueue* pq, int key) {
    int pos = pq->index[key];
    return (pos != -1) ? pq->heap[pos].priority : INT_MAX;
}

int pq_contains(PriorityQueue* pq, int key) {
    return key < pq->key_range && pq->index[key] != -1;
}

int pq_get_min_key(PriorityQueue* pq) {
    return pq_is_empty(pq) ? -1 : pq->heap[0].key;
}

void* pq_get_data(PriorityQueue* pq, int key) {
    int pos = pq->index[key];
    return (pos != -1) ? pq->heap[pos].data : NULL;
}