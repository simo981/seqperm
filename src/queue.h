#pragma once
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdatomic.h>
#define QUEUE_LEN 128

#define UNDEF(NAME, VAR) \
  if (VAR == NULL)       \
  {                      \
    perror(#NAME);       \
    exit(EXIT_FAILURE);  \
  }

#define FREE_P(p) \
  ({              \
    if (p)        \
    {             \
      free(p);    \
    }             \
  })

typedef struct input_aux_perm
{
  char **aux;
  size_t len;
} input_t;

typedef struct Queue
{
  input_t **words;
  atomic_long head;
  size_t tail;
  size_t maxsize;
} Queue_t;

Queue_t *init_queue(size_t size);
void push_queue(Queue_t *Q, char **aux, size_t len);
input_t *pop_queue(Queue_t *Q);
void free_queue(Queue_t *Q);
void resize(Queue_t *Q);
Queue_t *default_init_queue();
