#include "queue.h"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Queue_t *default_init_queue()
{
  return init_queue(QUEUE_LEN);
}

Queue_t *init_queue(size_t size)
{
  Queue_t *Q = (Queue_t *)malloc(sizeof(Queue_t));
  UNDEF(Q, "POINTER_QUEUE_INIT_FAIL");
  Q->words = (input_t **)calloc(size, sizeof(input_t *));
  UNDEF(Q, "BUCKET_QUEUE_INIT_FAIL");
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
  if (Q->tail >= Q->maxsize)
  {
    resize(Q);
  }
  Q->words[Q->tail++] = inQueue;
}

input_t *pop_queue(Queue_t *Q)
{
  input_t *aux = NULL;
  long idx = atomic_fetch_sub(&Q->head, 1);
  aux = idx >= 0 ? Q->words[idx] : NULL;
  return aux;
}

void resize(Queue_t *Q)
{
  size_t old_size = Q->maxsize;
  Q->maxsize *= 2;
  Q->words = (input_t **)realloc(Q->words, sizeof(input_t *) * Q->maxsize);
  memset(Q->words + old_size, 0x0, sizeof(input_t *) * (Q->maxsize - old_size));
}
