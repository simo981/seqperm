#ifndef QUEUE
#define QUEUE
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdatomic.h>

typedef struct input_aux_perm {
  char **aux;
  size_t len;
} input_t;

typedef struct Queue {
  input_t **words;
  atomic_uint head;
  size_t tail;
  size_t maxsize;
} Queue_t;

Queue_t *init_queue(size_t size);
void push_queue(Queue_t *Q, char **aux, size_t len);
input_t *pop_queue(Queue_t *Q);
void free_queue(Queue_t *Q);
#endif
