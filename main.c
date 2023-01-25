#include "main.h"
#include "queue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define BUFF 512
#define T 1
#define F 0

static size_t max_len;
static size_t min_len;
static char **dict;
static size_t word_size;
static Queue_t **all_queues = NULL;
static sig_atomic_t first_maiusc = F;
static sig_atomic_t not_number_start = F;

void print_out(char **arr, size_t size) {
  if (not_number_start) {
    if (arr[0][0] >= 48 && arr[0][0] <= 57) {
      return;
    }
  }
  char finalString[BUFF] = {0x0};
  size_t point = 0;
  for (size_t i = 0; i < size; i++) {
    memccpy(finalString + point, arr[i], '\0', strlen(arr[i]));
    point += strlen(arr[i]);
  }
  memcpy(finalString + point, "\n", 1);
  write(1, finalString, strlen(finalString));
  if (first_maiusc) {
    if (arr[0][0] >= 48 && arr[0][0] <= 57) {
      return;
    } else {
      finalString[0] -= 32;
    }
    write(1, finalString, strlen(finalString));
  }
}

void swap_p(char **f, char **s) {
  char *t = *f;
  *f = *s;
  *s = t;
}

void seq_perm(char **arr, size_t size) {
  unsigned short *P = (unsigned short *)calloc(size, sizeof(unsigned short));
  size_t i = 1;
  size_t j;
  while (i < size) {
    if (P[i] < i) {
      j = (i & 1) * P[i];
      swap_p(&arr[i], &arr[j]);
      print_out(arr, size);
      P[i]++;
      i = 1;
    } else {
      P[i] = 0;
      i++;
    }
  }
  free(P);
}

void *thread_perm(void *in) {
  size_t queuePos = (size_t)in;
  input_t *words;
  for (;;) {
    words = pop_queue(all_queues[queuePos]);
    if (words == NULL) {
      break;
    }
    print_out(words->aux, words->len);
    seq_perm(words->aux, words->len);
    for (size_t i = 0; i < words->len; i++) {
      free(words->aux[i]);
    }
    free(words->aux);
    free(words);
  }
  return NULL;
}

void gen_bin(unsigned short *arr, size_t s, size_t occ) {
  if (occ > max_len) {
    return;
  }
  if (s == 0) {
    if (occ >= min_len) {
      char **auxPerm = (char **)malloc(sizeof(char *) * occ);
      size_t pointer = 0;
      for (size_t j = 0; j < word_size; j++) {
        if (arr[j] == 1) {
          auxPerm[pointer] = (char *)calloc(strlen(dict[j]) + 1, sizeof(char));
          memccpy(auxPerm[pointer], dict[j], '\0', strlen(dict[j]));
          pointer++;
        }
      }
      push_queue(all_queues[occ - 1], auxPerm, occ);
    }
  } else {
    arr[s - 1] = 0;
    gen_bin(arr, s - 1, occ++);
    arr[s - 1] = 1;
    gen_bin(arr, s - 1, occ);
  }
  return;
}

int main(int argc, char **argv) {
  if (argc < 5) {
    exit_usage("Missing Parameters");
  }
  int opt;
  size_t thread_n;
  size_t queue_n;
  while ((opt = getopt(argc, argv, "nfs:e:")) != -1) {
    switch (opt) {
    case 'f': {
      first_maiusc = T;
      break;
    }
    case 'n': {
      not_number_start = T;
      break;
    }
    case 's': {
      err("Wrong min_len Param", min_len, atol(optarg));
      le0("min_len can't be less than 0", min_len);
      break;
    }
    case 'e': {
      err("Wrong max_len Param", max_len, atol(optarg));
      le0("max_len can't be less than 0", max_len);
      break;
    }
    case '?': {
      exit_usage("Wrong Params");
      break;
    }
    default: {
      exErr("Optarg Fatal Error");
    }
    }
  }
  word_size = argc - optind;
  queue_n = max_len;
  thread_n = (size_t)(max_len * (max_len + 1)) / 2;
  all_queues = (Queue_t **)malloc(sizeof(Queue_t *) * queue_n);
  for (size_t i = 0; i < queue_n; i++) {
    all_queues[i] = init_queue();
  }
  char **input_words = (char **)malloc(sizeof(char *) * word_size);
  for (size_t i = 0; i < word_size; i++) {
    input_words[i] = argv[optind];
    optind++;
  }
  dict = input_words;
  unsigned short *bin =
      (unsigned short *)calloc(word_size, sizeof(unsigned short));
  pthread_t tworker[thread_n];
  size_t pos = 0;
  for (size_t i = 0; i < queue_n; i++) {
    size_t j = i + 1;
    while (j > 0) {
      pthread_create(&tworker[pos], NULL, thread_perm, (void *)i);
      j--;
      pos++;
    }
  }
  gen_bin(bin, word_size, 0);
  free(bin);
  set_no_write();
  for (size_t i = 0; i < queue_n; i++) {
    broadcast_empty(all_queues[i]);
  }
  for (size_t i = 0; i < thread_n; i++) {
    pthread_join(tworker[i], NULL);
  }
  free(input_words);
  for (size_t i = 0; i < queue_n; i++) {
    free_queue(all_queues[i]);
    free(all_queues[i]);
  }
  free(all_queues);
  return 0;
}
