#include "main.h"
#include "queue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>

static size_t max_len = 0;
static size_t min_len = 0;
static char **dict;
static size_t word_size;
static Queue_t **all_queues = NULL;
static char **connectors = NULL;
static char leet_map[128] = {0x0};
static char **last = NULL;
static size_t connectors_size = 0;
static size_t last_size = 0;
static const char *connector_placeholder = "|";
static bool leet_vowel = false;
static bool leet_full = false;
static bool upper_first = false;
static bool upper_full = false;
static bool only_transform = false;

unsigned **binomialCoefficient(size_t n, size_t k)
{
  unsigned **C = (unsigned **)calloc(n + 1, sizeof(unsigned *));
  for (size_t i = 0; i <= n; i++)
  {
    C[i] = (unsigned *)calloc(k + 1, sizeof(unsigned));
  }
  for (size_t i = 0; i <= n; i++)
  {
    for (size_t j = 0; j <= k && j <= i; j++)
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
        {"only_transformations", required_argument, 0, 'p'},
        {"leet", required_argument, 0, 'k'},
        {"connectors", required_argument, 0, 'c'},
        {"start", required_argument, 0, 's'},
        {"end", required_argument, 0, 'e'},
        {0, 0, 0, 0}};

inline size_t add_string(char *buff[BUFF], size_t idx, char *to_push, size_t to_push_len)
{
  char *raw_copy = (char *)malloc(sizeof(char) * (to_push_len + 1));
  memccpy(raw_copy, to_push, '\0', to_push_len);
  raw_copy[to_push_len] = '\0';
  buff[idx] = raw_copy;
  return idx + 1;
};

inline void print_out(char **arr, size_t size)
{
  char finalString[BUFF] = {0x0};
  char *all_strings[BUFF] = {0x0};
  size_t run_len = 0, strings_len = 0;
  size_t lengths[size];
  size_t saved_len = 0;

  for (size_t i = 0; i < size; i++)
  {
    lengths[i] = strlen(arr[i]);
    memccpy(finalString + run_len, arr[i], '\0', lengths[i]);
    run_len += lengths[i];
  }
  strings_len = add_string(all_strings, strings_len, finalString, run_len);

  if (connectors && size >= 2)
  {
    char connectorString[BUFF] = {0x0};
    size_t cumulative_len = 0;
    for (size_t i = 0; i < size - 1; i++)
    {
      cumulative_len += lengths[i];
      memccpy(connectorString, finalString, '\0', cumulative_len);
      connectorString[cumulative_len] = connector_placeholder[0];
      memccpy(connectorString + cumulative_len + 1, finalString + cumulative_len, '\0', run_len - cumulative_len + 1);
      for (size_t y = 0; y < connectors_size; y++)
      {
        connectorString[cumulative_len] = connectors[y][0];
        strings_len = add_string(all_strings, strings_len, connectorString, run_len + 1);
      }
    }
  }

  if (last)
  {
    saved_len = strings_len;
    for (size_t i = 0; i < saved_len; i++)
    {
      size_t size_of_number;
      const char *my_str = all_strings[i];
      size_t copy_len = strlen(my_str);
      char copy[BUFF] = {0x0};
      memccpy(copy, my_str, '\0', copy_len);
      for (size_t j = 0; j < last_size; j++)
      {
        size_of_number = strlen(last[j]);
        memccpy(copy + copy_len, last[j], '\0', size_of_number);
        strings_len = add_string(all_strings, strings_len, copy, copy_len + size_of_number);
      }
    }
  }

  if (only_transform)
  {
    saved_len = strings_len;
    for (size_t i = 0; i < saved_len; i++)
    {
        const char *my_str = all_strings[i];
        size_t copy_len = strlen(my_str);
        char copy[BUFF] = {0x0};
        memccpy(copy, my_str, '\0', copy_len);
        bool u = upper_encode(copy);
        bool l = leet_encode(copy);
        strings_len = add_string(all_strings, strings_len, copy, copy_len);
    }
  }
  else
  {
    saved_len = strings_len;
    for (size_t i = 0; i < saved_len; i++)
    {
      const char *my_str = all_strings[i];
      size_t copy_len = strlen(my_str);
      char copy[BUFF] = {0x0};
      memccpy(copy, my_str, '\0', copy_len);
      if (leet_encode(copy))
      {
        strings_len = add_string(all_strings, strings_len, copy, copy_len);
      }
    }

    saved_len = strings_len;
    for (size_t i = 0; i < saved_len; i++)
    {
      const char *my_str = all_strings[i];
      size_t copy_len = strlen(my_str);
      char copy[BUFF] = {0x0};
      memccpy(copy, my_str, '\0', copy_len);
      if (upper_encode(copy))
      {
        strings_len = add_string(all_strings, strings_len, copy, copy_len);
      }
    }
  }

  saved_len = only_transform ? saved_len : 0;
  for (size_t i = 0; i < strings_len; i++)
  {
      if (i >= saved_len)
      {
          printf("%s\n", all_strings[i]);
      }
      free(all_strings[i]);
  }
}

inline bool leet_encode(char *str)
{
  bool encoded = false;
  if(leet_vowel || leet_full)
  {  while (*str != '\0')
    {
      if (leet_map[*str])
      {
        *str = leet_map[*str];
        encoded = true;
      }
      str++;
    }
  }
  return encoded;
}

inline bool upper_encode(char *str)
{
  bool encoded = false;
  if (upper_full)
  {
    for (size_t j = 0; j < strlen(str); j++)
    {
      if (str[j] >= 97 && str[j] <= 122)
      {
        str[j] -= 32;
        encoded = true;
      }
    }
  }
  else if (upper_first && str[0] >= 97 && str[0] <= 122)
  {
    str[0] -= 32;
    encoded = true;
  }
  return encoded;
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
}

int main(int argc, char **argv)
{
  int c, option_index = 0;
  size_t thread_n, queue_n;
  while ((c = getopt_long(argc, argv, "u:l:p:k:c:s:e:", long_options, &option_index)) != -1)
  {
    switch (c)
    {
    case 'k':
    {
      if (strcmp(optarg, "full") == 0)
      {
        leet_full = true;
      }
      else if (strcmp(optarg, "vowel") == 0)
      {
        leet_vowel = true;
      }

      break;
    }
    case 'u':
    {
      if (strcmp(optarg, "full") == 0)
      {
        upper_full = true;
      }
      else if (strcmp(optarg, "first") == 0)
      {
        upper_first = true;
      }

      break;
    }
    case 'p':
    {
      if (optarg[0] == 'Y' || optarg[0] == 'y')
      {
        only_transform = true;
      }
      else if (optarg[0] == 'N' || optarg[0] == 'n')
      {
        only_transform = false;
      }
      else{
        free_inputs_optind();
        exit_usage("wrong parameters");
        break;
      }

      break;
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
    char *token = strtok(optarg, ",");
    size_t x = 0;
    while (token != NULL)
    {
      last[x] = (char *)malloc(sizeof(char) * (strlen(token) + 1));
      strcpy(last[x], token);
      token = strtok(NULL, ",");
      x++;
    }
    last_size = x;
    break;
  }

    case 's':
    {
      ERR("min_len can't be less than 0 or null", min_len, strtoul(optarg, NULL, 10));
      break;
    }
    case 'e':
    {
      ERR("max_len can't be less than 0 or null", max_len, strtoul(optarg, NULL, 10));
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

  // check if start or end are provided by the user
  if (!min_len || !max_len)
  {
    free_inputs_optind();
    exit_usage("start and end must be stated");
  }
  else
  {
    LOW("max_len must be greater than min_len", max_len, min_len);
  }

  // check if words are provided by the user
  if (optind == argc)
  {
    free_inputs_optind();
    exit_usage("Words after are not provided");
  }

  if(only_transform && !leet_vowel && !leet_full && !upper_first && !upper_full)
  {
      free_inputs_optind();
      exit_usage("--only_transformations require at least one transformation");
  }

  leet_map['a'] = '4';
  leet_map['A'] = '4';
  leet_map['e'] = '3';
  leet_map['E'] = '3';
  leet_map['i'] = '1';
  leet_map['I'] = '1';
  leet_map['o'] = '0';
  leet_map['O'] = '0';
  if (leet_full)
  {
    leet_map['s'] = '5';
    leet_map['S'] = '5';
    leet_map['t'] = '7';
    leet_map['T'] = '7';
    leet_map['g'] = '9';
    leet_map['G'] = '9';
    leet_map['z'] = '2';
    leet_map['Z'] = '2';
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
  for (size_t i = 0; i < word_size + 1; i++)
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
  memset(&tworker, 0x0, sizeof(tworker));
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
