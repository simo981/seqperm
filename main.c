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

static size_t maxLen;
static size_t minLen;
static char **dict;
static size_t wordSize;
static Queue_t **all_queues = NULL;
static sig_atomic_t firstMaiusc = F;
static sig_atomic_t notNumberStart = F;

void printOut(char **arr, size_t size) {
  if (notNumberStart) {
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
  if (firstMaiusc) {
    if (arr[0][0] >= 48 && arr[0][0] <= 57) {
      return;
    } else {
      finalString[0] -= 32;
    }
    write(1, finalString, strlen(finalString));
  }
}

void swapPP(char **f, char **s) {
  char *t = *f;
  *f = *s;
  *s = t;
}

void seqPerm(char **arr, size_t size) {
  unsigned short *P = (unsigned short *)calloc(size, sizeof(unsigned short));
  size_t i = 1;
  size_t j;
  while (i < size) {
    if (P[i] < i) {
      j = (i & 1) * P[i];
      swapPP(&arr[i], &arr[j]);
      printOut(arr, size);
      P[i]++;
      i = 1;
    } else {
      P[i] = 0;
      i++;
    }
  }
  free(P);
}

void *threadPerm(void *in) {
  size_t queuePos = (size_t)in;
  input_t *words;
  for (;;) {
    words = pop_queue(all_queues[queuePos]);
    if (words == NULL) {
      break;
    }
    printOut(words->aux, words->len);
    seqPerm(words->aux, words->len);
    for (size_t i = 0; i < words->len; i++) {
      free(words->aux[i]);
    }
    free(words->aux);
    free(words);
  }
  return NULL;
}

void genBin(unsigned short *arr, size_t s, size_t occ) {
  if (occ > maxLen) {
    return;
  }
  if (s == 0) {
    if (occ >= minLen) {
      char **auxPerm = (char **)malloc(sizeof(char *) * occ);
      size_t pointer = 0;
      for (size_t j = 0; j < wordSize; j++) {
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
    genBin(arr, s - 1, occ++);
    arr[s - 1] = 1;
    genBin(arr, s - 1, occ);
  }
  return;
}

int main(int argc, char **argv) {
  if (argc < 5) {
    exitUsage("Missing Parameters");
  }
  int opt;
  size_t threadN;
  size_t queueN;
  while ((opt = getopt(argc, argv, "nfs:e:")) != -1) {
    switch (opt) {
    case 'f': {
      firstMaiusc = T;
      break;
    }
    case 'n': {
      notNumberStart = T;
      break;
    }
    case 's': {
      err("Wrong minLen Param", minLen, atol(optarg));
      le0("minLen can't be less than 0", minLen);
      break;
    }
    case 'e': {
      err("Wrong maxLen Param", maxLen, atol(optarg));
      le0("maxLen can't be less than 0", maxLen);
      break;
    }
    case '?': {
      exitUsage("Wrong Params");
      break;
    }
    default: {
      exErr("Optarg Fatal Error");
    }
    }
  }
  wordSize = argc - optind;
  queueN = maxLen;
  threadN = (size_t)(maxLen * (maxLen + 1)) / 2;
  all_queues = (Queue_t **)malloc(sizeof(Queue_t *) * queueN);
  for (size_t i = 0; i < queueN; i++) {
    all_queues[i] = init_queue();
  }
  char **input_words = (char **)malloc(sizeof(char *) * wordSize);
  for (size_t i = 0; i < wordSize; i++) {
    input_words[i] = argv[optind];
    optind++;
  }
  dict = input_words;
  unsigned short *bin =
      (unsigned short *)calloc(wordSize, sizeof(unsigned short));
  pthread_t tworker[threadN];
  size_t pos = 0;
  for (size_t i = 0; i < queueN; i++) {
    size_t j = i + 1;
    while (j > 0) {
      pthread_create(&tworker[pos], NULL, threadPerm, (void *)i);
      j--;
      pos++;
    }
  }
  genBin(bin, wordSize, 0);
  free(bin);
  for (size_t i = 0; i < queueN; i++) {
    nowrite_queue(all_queues[i]);
  }
  for (size_t i = 0; i < threadN; i++) {
    pthread_join(tworker[i], NULL);
  }
  free(input_words);
  for (size_t i = 0; i < queueN; i++) {
    free_queue(all_queues[i]);
    free(all_queues[i]);
  }
  free(all_queues);
  return 0;
}
