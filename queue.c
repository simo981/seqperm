#include "queue.h"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Queue_t *init_queue(size_t size)
{
  Queue_t *Q = (Queue_t *)malloc(sizeof(Queue_t));
  if (Q == NULL)
  {
    perror("POINTER_QUEUE_INIT_FAIL");
    exit(EXIT_FAILURE);
  }
  Q->words = (input_t **)malloc(sizeof(input_t *) * size);
  if (Q->words == NULL)
  {
    perror("BUCKET_QUEUE_INIT_FAIL");
    exit(EXIT_FAILURE);
  }
  Q->maxsize = size;
  Q->head = 0;
  Q->tail = 0;
  return Q;
}

void push_queue(Queue_t *Q, char **aux, size_t len)
{
  input_t *inQueue = (input_t *)malloc(sizeof(input_t));
  inQueue->aux = aux;
  inQueue->len = len;
  Q->words[Q->tail++] = inQueue;
}

input_t *pop_queue(Queue_t *Q)
{
  input_t *aux = NULL;
  unsigned int idx = atomic_fetch_add(&Q->head, 1);
  aux = idx < Q->maxsize ? Q->words[idx] : NULL;
  return aux;
}

void free_queue(Queue_t *Q)
{
  free(Q->words);
}
