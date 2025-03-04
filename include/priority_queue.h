//
// Created by Andrew Boun on 2/24/25.
//

#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

typedef struct PQNode {
    int key;        // Key used for indexing (e.g., vertex id)
    void* data;     // Associated data
    int priority;   // Priority value
} PQNode;

typedef struct PriorityQueue {
    PQNode* heap;   // Binary heap array
    int* index;     // Maps key -> heap position
    int* rev_index; // Maps heap position -> key
    int size;       // Current size
    int capacity;   // Maximum capacity
    int key_range;  // Maximum key value + 1
    void* (*copy)(const void*);    // Copy function for data
    void (*destroy)(void*);        // Cleanup function for data
} PriorityQueue;

// Create new priority queue that can handle keys from 0 to key_range-1
PriorityQueue* pq_create(int capacity, int key_range, 
                        void* (*copy)(const void*),
                        void (*destroy)(void*));

void pq_destroy(PriorityQueue* pq);
int pq_is_empty(PriorityQueue* pq);

// Check if a key exists in the queue
int pq_contains(PriorityQueue* pq, int key);

// Insert/update operations now take a key instead of comparing data
void pq_insert(PriorityQueue* pq, int key, void* data, int priority);
void* pq_extract_min(PriorityQueue* pq);
void pq_update_priority(PriorityQueue* pq, int key, int new_priority);
int pq_get_priority(PriorityQueue* pq, int key);

// New operations specific to indexed PQ
int pq_get_min_key(PriorityQueue* pq);
void* pq_get_data(PriorityQueue* pq, int key);

#endif