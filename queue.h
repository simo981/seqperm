#ifndef QUEUE
#define QUEUE
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#define MAXLEN 4096

typedef struct input_aux_perm {
  char **aux;
  size_t len;
} input_t;

typedef struct Queue {
  input_t **words;
  size_t head;
  size_t tail;
  size_t maxsize;
  size_t actualsize;
  sig_atomic_t no_write;
  pthread_mutex_t lock;
  pthread_cond_t empty;
  pthread_cond_t full;
} Queue_t;

Queue_t *init_queue(void);
void push_queue(Queue_t *Q, char **aux, size_t len);
input_t *pop_queue(Queue_t *Q);
void free_queue(Queue_t *Q);
void set_no_write(Queue_t *Q);
void broadcast_empty(Queue_t *Q);
#endif
