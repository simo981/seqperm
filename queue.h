#ifndef QUEUE
#define QUEUE
#include <pthread.h>
#include <stddef.h>
#define MAXLEN 4096

static sig_atomic_t finished = 0;

typedef struct input_aux_perm {
  char **aux;
  size_t len;
} input_t;

typedef struct Queue {
  input_t** words;
  size_t head;
  size_t tail;
  size_t maxsize;
  size_t actualsize;
  pthread_mutex_t lock;
  pthread_cond_t empty;
  pthread_cond_t full;
} Queue_t;

Queue_t *init_queue(void);
void push_queue(Queue_t *Q, char **aux, size_t len);
input_t *pop_queue(Queue_t *Q);
void nowrite_queue(Queue_t *Q);
void free_queue(Queue_t *Q);

#endif
