#include "queue.h"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Queue_t *init_queue() {
  Queue_t *Q = (Queue_t *)malloc(sizeof(Queue_t));
  if (Q == NULL) {
    perror("POINTER_QUEUE_INIT_FAIL");
    exit(EXIT_FAILURE);
  }
  Q->words = (input_t **)malloc(sizeof(input_t *) * MAXLEN);
  if (Q->words == NULL) {
    perror("BUCKET_QUEUE_INIT_FAIL");
    exit(EXIT_FAILURE);
  }
  pthread_mutex_init(&Q->lock, NULL);
  pthread_cond_init(&Q->full, NULL);
  pthread_cond_init(&Q->empty, NULL);
  Q->maxsize = MAXLEN;
  Q->head = 0;
  Q->tail = 0;
  Q->actualsize = 0;
  return Q;
}

void push_queue(Queue_t *Q, char **aux, size_t len) {
  pthread_mutex_lock(&Q->lock);
  while (Q->actualsize == Q->maxsize) {
    pthread_cond_wait(&Q->full, &Q->lock);
  }
  input_t *inQueue = (input_t *)malloc(sizeof(input_t));
  inQueue->aux = aux;
  inQueue->len = len;
  Q->words[Q->tail] = inQueue;
  Q->tail = (Q->tail + 1 >= Q->maxsize) ? 0 : Q->tail + 1;
  Q->actualsize++;
  pthread_cond_broadcast(&Q->empty);
  pthread_mutex_unlock(&Q->lock);
}

input_t *pop_queue(Queue_t *Q) {
  input_t *aux = NULL;
  pthread_mutex_lock(&Q->lock);
  while (Q->actualsize == 0) {
    if (no_write) {
      pthread_mutex_unlock(&Q->lock);
      return NULL;
    }
    pthread_cond_wait(&Q->empty, &Q->lock);
  }
  aux = Q->words[Q->head];
  Q->head = (Q->head + 1 >= Q->maxsize) ? 0 : Q->head + 1;
  Q->actualsize--;
  pthread_cond_signal(&Q->full);
  pthread_mutex_unlock(&Q->lock);
  return aux;
}

void free_queue(Queue_t *Q) {
  pthread_mutex_lock(&Q->lock);
  pthread_cond_destroy(&Q->full);
  pthread_cond_destroy(&Q->empty);
  pthread_mutex_destroy(&Q->lock);
  free(Q->words);
  pthread_mutex_unlock(&Q->lock);
}

void set_no_write() { no_write = 1; }

void broadcast_empty(Queue_t *Q) { pthread_cond_broadcast(&Q->empty); }
