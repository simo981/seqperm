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
static size_t connectors_size = 0;
static char *connectors[3] = {",", ".", "-"};
static char *numbers[3] = {"!", "1", "72"};
static const char *connector_placeholder = "|";

inline void print_f_maiusc(char **arr, char *string)
{
  if (arr[0][0] >= 48 && arr[0][0] <= 57)
  {
    return;
  }
  else
  {
    string[0] -= 32;
  }
  printf("%s", string);
};

inline void print_number(char **arr, char *finalString, size_t run_len)
{
  if (numbers)
  {
    size_t size_of_number;
    for (size_t j = 0; j < 3; j++)
    {
      size_of_number = strlen(numbers[j]);
      memccpy(finalString + run_len, numbers[j], '\0', size_of_number);
      memccpy(finalString + run_len + size_of_number, "\n", '\0', 2);
      printf("%s", finalString);
      if (first_maiusc)
      {
        print_f_maiusc(arr, finalString);
      }
    }
  }
};

inline void print_out(char **arr, size_t size, size_t *lengths)
{
  if (not_number_start)
  {
    if (arr[0][0] >= 48 && arr[0][0] <= 57)
    {
      return;
    }
  }
  char finalString[BUFF] = {0x0};
  size_t run_len = 0;
  for (size_t i = 0; i < size; i++)
  {
    memccpy(finalString + run_len, arr[i], '\0', lengths[i]);
    run_len += lengths[i];
  }
  memcpy(finalString + run_len, "\n", 2);
  printf("%s", finalString);
  if (connectors)
  {
    char connectorString[BUFF] = {0x0};
    size_t cumulative_len = 0;
    for (size_t i = 0; i < size - 1; i++)
    {
      cumulative_len += lengths[i];
      memccpy(connectorString, finalString, '\0', cumulative_len);
      memccpy(connectorString + cumulative_len, connector_placeholder, '\0', 1);
      memccpy(connectorString + cumulative_len + 1, finalString + cumulative_len, '\0', run_len - cumulative_len + 2);
      for (size_t y = 0; y < 3; y++)
      {
        memccpy(connectorString + cumulative_len, connectors[y], '\0', 1);
        printf("%s", connectorString);
        if (first_maiusc)
        {
          print_f_maiusc(arr, connectorString);
        }
        if (numbers)
        {
          print_number(arr, connectorString, run_len + 1);
        }
      }
    }
  }
  if (first_maiusc)
  {
    print_f_maiusc(arr, finalString);
  }
  if (numbers)
  {
    print_number(arr, finalString, run_len);
  }
}

inline void swap_p(char **f, char **s)
{
  char *t = *f;
  *f = *s;
  *s = t;
}

void seq_perm(char **arr, size_t size, size_t *lengths)
{
  unsigned short *P = (unsigned short *)calloc(size, sizeof(unsigned short));
  size_t i = 1, j;
  while (i < size)
  {
    if (P[i] < i)
    {
      j = (i & 1) * P[i];
      swap_p(&arr[i], &arr[j]);
      print_out(arr, size, lengths);
      P[i]++;
      i = 1;
    }
    else
    {
      P[i] = 0;
      i++;
    }
  }
  free(P);
}

void *thread_perm(void *in)
{
  size_t queuePos = (size_t)in;
  input_t *words;
  for (;;)
  {
    words = pop_queue(all_queues[queuePos]);
    if (words == NULL)
    {
      break;
    }
    size_t run_len = 1;
    size_t *lengths = (size_t *)malloc(sizeof(size_t) * words->len);
    for (size_t i = 0; i < words->len; i++)
    {
      lengths[i] = strlen(words->aux[i]);
    }
    print_out(words->aux, words->len, lengths);
    seq_perm(words->aux, words->len, lengths);
    for (size_t i = 0; i < words->len; i++)
    {
      free(words->aux[i]);
    }
    free(lengths);
    free(words->aux);
    free(words);
  }
  return NULL;
}

void gen_bin(unsigned short *arr, size_t s, size_t occ)
{
  if (occ > max_len)
  {
    return;
  }
  if (s == 0)
  {
    if (occ >= min_len)
    {
      char **auxPerm = (char **)malloc(sizeof(char *) * occ);
      size_t pointer = 0;
      for (size_t j = 0; j < word_size; j++)
      {
        if (arr[j] == 1)
        {
          auxPerm[pointer] = (char *)calloc(strlen(dict[j]) + 1, sizeof(char));
          memccpy(auxPerm[pointer], dict[j], '\0', strlen(dict[j]));
          pointer++;
        }
      }
      push_queue(all_queues[occ - 1], auxPerm, occ);
    }
  }
  else
  {
    arr[s - 1] = 0;
    gen_bin(arr, s - 1, occ++);
    arr[s - 1] = 1;
    gen_bin(arr, s - 1, occ);
  }
  return;
}

int main(int argc, char **argv)
{
  if (argc < 5)
  {
    exit_usage("Missing Parameters");
  }
  int opt;
  size_t thread_n;
  size_t queue_n;
  while ((opt = getopt(argc, argv, "nfs:e:")) != -1)
  {
    switch (opt)
    {
    case 'f':
    {
      first_maiusc = T;
      break;
    }
    case 'n':
    {
      not_number_start = T;
      break;
    }
    case 's':
    {
      err("Wrong min_len Param", min_len, atol(optarg));
      le0("min_len can't be less than 0", min_len);
      break;
    }
    case 'e':
    {
      err("Wrong max_len Param", max_len, atol(optarg));
      le0("max_len can't be less than 0", max_len);
      break;
    }
    case '?':
    {
      exit_usage("Wrong Params");
      break;
    }
    default:
    {
      exErr("Optarg Fatal Error");
    }
    }
  }
  word_size = argc - optind;
  queue_n = max_len;
  thread_n = (size_t)(max_len * (max_len + 1)) / 2;
  all_queues = (Queue_t **)malloc(sizeof(Queue_t *) * queue_n);
  for (size_t i = 0; i < queue_n; i++)
  {
    all_queues[i] = init_queue();
  }
  char **input_words = (char **)malloc(sizeof(char *) * word_size);
  for (size_t i = 0; i < word_size; i++)
  {
    input_words[i] = argv[optind];
    optind++;
  }
  dict = input_words;
  unsigned short *bin =
      (unsigned short *)calloc(word_size, sizeof(unsigned short));
  pthread_t tworker[thread_n];
  size_t pos = 0;
  for (size_t i = 0; i < queue_n; i++)
  {
    size_t j = i + 1;
    while (j > 0)
    {
      pthread_create(&tworker[pos], NULL, thread_perm, (void *)i);
      j--;
      pos++;
    }
  }
  gen_bin(bin, word_size, 0);
  free(bin);
  for (size_t i = 0; i < queue_n; i++)
  {
    set_no_write(all_queues[i]);
    broadcast_empty(all_queues[i]);
  }
  for (size_t i = 0; i < thread_n; i++)
  {
    pthread_join(tworker[i], NULL);
  }
  free(input_words);
  for (size_t i = 0; i < queue_n; i++)
  {
    free_queue(all_queues[i]);
    free(all_queues[i]);
  }
  free(all_queues);
  return 0;
}
