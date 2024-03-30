#include "main.h"
#include "queue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>

static size_t max_len;
static size_t min_len;
static char **dict;
static size_t word_size;
static Queue_t **all_queues = NULL;
static char **connectors = NULL;
static char **last = NULL;
static size_t connectors_size = 0;
static size_t last_size = 0;
static const char *connector_placeholder = "|";
static int first_maiusc = false;

unsigned **binomialCoefficient(size_t n, size_t k)
{
  unsigned **C = (unsigned **)malloc(sizeof(unsigned *) * (n + 1));
  for (int i = 0; i <= n; i++)
  {
    C[i] = (unsigned *)malloc(sizeof(unsigned) * (k + 1));
  }
  for (int i = 0; i <= n; i++)
  {
    for (int j = 0; j <= k && j <= i; j++)
    {
      if (j == 0 || j == i)
      {
        C[i][j] = 1;
      }
      else
      {
        C[i][j] = C[i - 1][j - 1] + C[i - 1][j];
      }
    }
  }
  return C;
}

void free_inputs_optind(void)
{
  if (connectors)
  {
    free(connectors);
  }
  if (last)
  {
    free(last);
  }
}

static struct option long_options[] =
    {
        {"upper", required_argument, 0, 'u'},
        {"last", required_argument, 0, 'l'},
        {"connectors", required_argument, 0, 'c'},
        {"start", required_argument, 0, 's'},
        {"end", required_argument, 0, 'e'},
        {0, 0, 0, 0}};

inline size_t add_string(char *buff[BUFF], size_t idx, char *to_push, size_t to_push_len)
{
  char *raw_copy = (char *)malloc(sizeof(char) * to_push_len);
  memccpy(raw_copy, to_push, '\0', to_push_len);
  buff[idx] = raw_copy;
  return idx + 1;
};

inline void print_out(char **arr, size_t size)
{
  char finalString[BUFF] = {0x0};
  char *all_strings[BUFF] = {0x0};
  size_t run_len = 0, strings_len = 0;
  size_t lengths[size];
  for (size_t i = 0; i < size; i++)
  {
    lengths[i] = strlen(arr[i]);
    memccpy(finalString + run_len, arr[i], '\0', lengths[i]);
    run_len += lengths[i];
  }
  finalString[run_len] = '\0';
  strings_len = add_string(all_strings, strings_len, finalString, run_len + 1);
  if (connectors)
  {
    char connectorString[BUFF] = {0x0};
    size_t cumulative_len = 0;
    for (size_t i = 0; i < size - 1; i++)
    {
      cumulative_len += lengths[i];
      memccpy(connectorString, finalString, '\0', cumulative_len);
      memccpy(connectorString + cumulative_len, connector_placeholder, '\0', 1);
      memccpy(connectorString + cumulative_len + 1, finalString + cumulative_len, '\0', run_len - cumulative_len + 1);
      for (size_t y = 0; y < connectors_size; y++)
      {
        memccpy(connectorString + cumulative_len, connectors[y], '\0', 1);
        strings_len = add_string(all_strings, strings_len, connectorString, run_len + 2);
      }
    }
  }
  if (last)
  {
    size_t saved_len = strings_len;
    for (size_t i = 0; i < saved_len; i++)
    {
      size_t size_of_number;
      const char *my_str = all_strings[i];
      char copy[BUFF] = {0x0};
      memccpy(copy, my_str, '\0', strlen(my_str));
      size_t copy_len = strlen(my_str);
      for (size_t j = 0; j < last_size; j++)
      {
        size_of_number = strlen(last[j]);
        memccpy(copy + copy_len, last[j], '\0', size_of_number);
        copy[copy_len + size_of_number] = '\0';
        strings_len = add_string(all_strings, strings_len, copy, copy_len + size_of_number + 1);
      }
    }
  }
  for (size_t i = 0; i < strings_len; i++)
  {
    printf("%s\n", all_strings[i]);
    if (first_maiusc && all_strings[i][0] >= 97 && all_strings[i][0] <= 122)
    {
      all_strings[i][0] -= 32;
      printf("%s\n", all_strings[i]);
    }
    free(all_strings[i]);
  }
}

inline void swap_p(char **f, char **s)
{
  char *t = *f;
  *f = *s;
  *s = t;
}

void seq_perm(char **arr, size_t size)
{
  unsigned short *P = (unsigned short *)calloc(size, sizeof(unsigned short));
  size_t i = 1, j;
  while (i < size)
  {
    if (P[i] < i)
    {
      j = (i & 1) * P[i];
      swap_p(&arr[i], &arr[j]);
      print_out(arr, size);
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
    print_out(words->aux, words->len);
    seq_perm(words->aux, words->len);
    for (size_t i = 0; i < words->len; i++)
    {
      free(words->aux[i]);
    }
    free(words->aux);
    free(words);
  }
  return NULL;
}

void gen_bin_perms(unsigned short *arr, size_t size, size_t idx, size_t max, size_t cur, size_t min)
{
  if (idx == size)
  {
    if (cur >= min && cur <= max)
    {
      char **auxPerm = (char **)malloc(sizeof(char *) * cur);
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
      push_queue(all_queues[cur - min], auxPerm, cur);
    }
    return;
  }
  arr[idx] = 0;
  gen_bin_perms(arr, size, idx + 1, max, cur, min);
  if (cur < max)
  {
    arr[idx] = 1;
    gen_bin_perms(arr, size, idx + 1, max, cur + 1, min);
  }
};

int main(int argc, char **argv)
{
  if (argc < 5)
  {
    exit_usage("missing parameters");
  }
  int c, option_index = 0;
  size_t thread_n, queue_n;
  while ((c = getopt_long(argc, argv, "l:c:s:e:u:", long_options, &option_index)) != -1)
  {
    switch (c)
    {
    case 'u':
    {
      if (optarg[0] == 'Y' || optarg[0] == 'y')
      {
        first_maiusc = true;
      }
    }
    case 'c':
    {
      connectors = (char **)malloc(sizeof(char *) * BUFF);
      char connector_to_copy[2] = {0x0};
      for (size_t i = 0; i < strlen(optarg); i++)
      {
        connector_to_copy[0] = optarg[i];
        connectors[i] = (char *)malloc(sizeof(char) * 2);
        memccpy(connectors[i], connector_to_copy, '\0', sizeof(char) * 2);
      }
      connectors_size = strlen(optarg);
      break;
    }
    case 'l':
    {
      last = (char **)malloc(sizeof(char *) * BUFF);
      char connector_to_copy[BUFF] = {0x0};
      size_t j = 0, x = 0;
      for (size_t i = 0; i < strlen(optarg); i++)
      {
        if (optarg[i] == ',' || (i + 1) == strlen(optarg))
        {
          if (j == 0)
          {
            connector_to_copy[j++] = optarg[i];
          }
          last[x] = (char *)malloc(sizeof(char) * ++j);
          memccpy(last[x++], connector_to_copy, '\0', j);
          memset(connector_to_copy, '\0', BUFF);
          j = 0;
        }
        else
        {
          connector_to_copy[j++] = optarg[i];
        }
      }
      last_size = x;
      break;
    }
    case 's':
    {
      ERR("wrong min_len parameter", min_len, atol(optarg));
      LE0("min_len can't be less than 0", min_len);
      break;
    }
    case 'e':
    {
      ERR("wrong max_len parameter", max_len, atol(optarg));
      LE0("max_len can't be less than 0", max_len);
      break;
    }
    case '?':
    {
      free_inputs_optind();
      exit_usage("wrong parameters");
      break;
    }
    default:
    {
      free_inputs_optind();
      perror("OPTARG FAILURE");
      exit(EXIT_FAILURE);
    }
    }
  }
  word_size = (size_t)argc - (size_t)optind;
  queue_n = max_len - min_len + 1;
  thread_n = (size_t)(max_len * (max_len + 1)) / 2;
  all_queues = (Queue_t **)malloc(sizeof(Queue_t *) * queue_n);
  unsigned **C = binomialCoefficient(word_size, max_len);
  for (size_t i = 0; i < queue_n; i++)
  {
    all_queues[i] = init_queue(C[word_size][min_len + i]);
  }
  for (size_t i = 0; i < max_len + 1; i++)
  {
    free(C[i]);
  }
  free(C);
  char **input_words = (char **)malloc(sizeof(char *) * word_size);
  for (size_t i = 0; i < word_size; i++)
  {
    input_words[i] = argv[optind];
    optind++;
  }
  dict = input_words;
  unsigned short *bin = (unsigned short *)calloc(word_size, sizeof(unsigned short));
  for (size_t i = 0; i < min_len; i++)
  {
    bin[i] = 1;
  }
  gen_bin_perms(bin, word_size, 0, max_len, 0, min_len);
  free(bin);
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
  for (size_t i = 0; i < thread_n; i++)
  {
    pthread_join(tworker[i], NULL);
  }
  for (size_t i = 0; i < queue_n; i++)
  {
    free_queue(all_queues[i]);
    free(all_queues[i]);
  }
  for (size_t i = 0; i < connectors_size; i++)
  {
    free(connectors[i]);
  }
  for (size_t i = 0; i < last_size; i++)
  {
    free(last[i]);
  }
  free_inputs_optind();
  free(all_queues);
  free(input_words);
  return 0;
}
