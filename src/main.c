#include "main.h"
#include "queue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>

static size_t max_len = 0, min_len = 0, word_size, connectors_size = 0, last_size = 0;
static char **dict, **connectors = NULL, **last = NULL;
static char leet_map[128] = {0x0};
static const char *connector_placeholder = "|";
static unsigned short ***delim_bins = NULL;
static modifiers_t bool_modifiers = {0x0};
static Queue_t *global_queue = NULL;

void gen_bin_to_arr(unsigned short *arr, size_t size, size_t idx, size_t max, size_t cur, size_t min, unsigned short ***out, size_t *out_size)
{
  if (idx == size)
  {
    unsigned short *copy = (unsigned short *)malloc(sizeof(unsigned short) * size);
    memcpy(copy, arr, sizeof(unsigned short) * size);
    out[size][*out_size] = copy;
    *out_size = *out_size + 1;
    return;
  }
  arr[idx] = 0;
  gen_bin_to_arr(arr, size, idx + 1, max, cur, min, out, out_size);
  if (cur < max)
  {
    arr[idx] = 1;
    gen_bin_to_arr(arr, size, idx + 1, max, cur + 1, min, out, out_size);
  }
}

void free_inputs_optind(void)
{
  FREE_P(connectors);
  FREE_P(last);
}

static struct option long_options[] =
    {
        {"upper", required_argument, 0, 'u'},
        {"last", required_argument, 0, 'l'},
        {"only_transformations", no_argument, 0, 'p'},
        {"reverse", required_argument, 0, 'r'},
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

inline bool palindrome(char *str, size_t len)
{
  char *p1 = str;
  char *p2 = str + len - 1;
  while (p1 <= p2)
  {
    if (*p1++ != *p2--)
    {
      return false;
    }
  }
  return true;
}

inline void reverse(char *str, size_t len)
{
  char *p1 = str;
  char *p2 = str + len - 1;
  while (p1 < p2)
  {
    char tmp = *p1;
    *p1++ = *p2;
    *p2-- = tmp;
  }
}

inline void print_out(char **arr, size_t size)
{
  char finalString[BUFF] = {0x0};
  char *all_strings[BUFF] = {0x0};
  size_t run_len = 0, strings_len = 0;
  size_t lengths[size];
  size_t saved_len = 0;
  size_t reverse_make_sense = false;
  for (size_t i = 0; i < size; i++)
  {
    lengths[i] = strlen(arr[i]);
    memccpy(finalString + run_len, arr[i], '\0', lengths[i]);
    run_len += lengths[i];
  }
  strings_len = add_string(all_strings, strings_len, finalString, run_len);
  if (bool_modifiers.reverse_words || bool_modifiers.reverse_full)
  {
    size_t one = 0, palindrome_count = 0;
    for (size_t i = 0; i < size; i++)
    {
      if (lengths[i] == 1)
      {
        one++;
      }
      else if (palindrome(arr[i], lengths[i]))
      {
        palindrome_count++;
      }
    }
    if (one != size && palindrome_count != size)
    {
      reverse_make_sense = true;
    }
    if (reverse_make_sense && bool_modifiers.reverse_words)
    {
      saved_len = strings_len;
      for (size_t i = 0; i < saved_len; i++)
      {
        char copy[BUFF] = {0x0};
        size_t copy_len = COPY_STRING(copy, all_strings[i]);
        reverse(copy, copy_len);
        strings_len = add_string(all_strings, strings_len, copy, copy_len);
      }
    }
  }
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
      char copy[BUFF] = {0x0};
      size_t copy_len = COPY_STRING(copy, all_strings[i]);
      for (size_t j = 0; j < last_size; j++)
      {
        size_of_number = strlen(last[j]);
        memccpy(copy + copy_len, last[j], '\0', size_of_number);
        strings_len = add_string(all_strings, strings_len, copy, copy_len + size_of_number);
      }
    }
  }
  if (bool_modifiers.leet_full || bool_modifiers.leet_vowel)
  {
    saved_len = strings_len;
    for (size_t i = 0; i < saved_len; i++)
    {
      char copy[BUFF] = {0x0};
      size_t copy_len = COPY_STRING(copy, all_strings[i]);
      if (leet_encode(copy))
      {
        strings_len = add_string(all_strings, strings_len, copy, copy_len);
      }
    }
  }
  if (bool_modifiers.upper_first || bool_modifiers.upper_full)
  {
    saved_len = strings_len;
    for (size_t i = 0; i < saved_len; i++)
    {
      char copy[BUFF] = {0x0};
      size_t copy_len = COPY_STRING(copy, all_strings[i]);
      if (upper_encode(copy))
      {
        strings_len = add_string(all_strings, strings_len, copy, copy_len);
      }
    }
  }
  if (reverse_make_sense && bool_modifiers.reverse_full)
  {
    saved_len = strings_len;
    for (size_t i = 0; i < saved_len; i++)
    {
      char copy[BUFF] = {0x0};
      size_t copy_len = COPY_STRING(copy, all_strings[i]);
      reverse(copy, copy_len);
      strings_len = add_string(all_strings, strings_len, copy, copy_len);
    }
  }
  saved_len = bool_modifiers.only_transform ? saved_len : 0;
  for (size_t i = 0; i < strings_len; i++)
  {
    if (i >= saved_len)
    {
      printf("%s\n", all_strings[i]);
    }
    FREE_P(all_strings[i]);
  }
}

inline bool leet_encode(char *str)
{
  bool encoded = false;
  while (*str != '\0')
  {
    if (leet_map[*str])
    {
      *str = leet_map[*str];
      encoded = true;
    }
    str++;
  }
  return encoded;
}

inline bool upper_encode(char *str)
{
  bool encoded = false;
  if (bool_modifiers.upper_full)
  {
    for (char *ptr = str; *ptr != '\0'; ptr++)
    {
      if (islower(*ptr))
      {
        *ptr = toupper((unsigned char)*ptr);
        encoded = true;
      }
    }
  }
  else if (bool_modifiers.upper_first && islower((unsigned char)str[0]))
  {
    str[0] = toupper((unsigned char)str[0]);
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
  unsigned short P[size];
  memset(&P, 0x0, sizeof(unsigned short) * size);
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
      P[i++] = 0;
    }
  }
}

void *thread_perm(void *in)
{
  input_t *words;
  for (;;)
  {
    words = pop_queue(global_queue);
    if (words == NULL)
    {
      break;
    }
    print_out(words->aux, words->len);
    seq_perm(words->aux, words->len);
    FREE_PP(words->aux, 0, words->len);
    FREE_P(words);
  }
  return NULL;
}

void gen_bin_perms(unsigned short *arr, size_t size, size_t idx, size_t max, size_t cur, size_t min)
{
  if (idx == size)
  {
    if (cur >= min && cur <= max)
    {
      char **auxPerm = (char **)calloc(cur, sizeof(char *));
      size_t pointer = 0, delim = 0;
      size_t idx_delim[word_size];
      for (size_t j = 0; j < word_size; j++)
      {
        if (arr[j] == 1)
        {
          char *p = strchr(dict[j], ',');
          if (p == NULL)
          {
            auxPerm[pointer++] = strdup(dict[j]);
          }
          else
          {
            idx_delim[delim++] = j;
          }
        }
      }
      if (delim)
      {
        delim_t delim_words[delim];
        memset(&delim_words, 0x0, sizeof(delim_t) * delim);
        for (size_t d = 0; d < delim; d++)
        {
          char *pp = strdup(dict[idx_delim[d]]);
          char *p = strtok(pp, ",");
          delim_words[d].p1 = strdup(p);
          p = strtok(NULL, ",");
          char *copy = (char *)calloc(strlen(delim_words[d].p1) + strlen(p) + 1, sizeof(char));
          copy = strcat(copy, delim_words[d].p1);
          copy = strcat(copy, p);
          delim_words[d].p2 = copy;
          FREE_P(pp);
        }
        size_t n_bin_delim = (size_t)(1 << delim);
        unsigned short **bins = delim_bins[delim];
        for (size_t i = 0; i < n_bin_delim; i++)
        {
          char **full_dict = (char **)malloc(sizeof(char *) * cur);
          for (size_t n = 0; n < pointer; n++)
          {
            full_dict[n] = strdup(auxPerm[n]);
          }
          size_t size_dict = pointer;
          unsigned short *x = bins[i];
          for (size_t j = 0; j < delim; j++)
          {
            delim_t *delim_word = &delim_words[j];
            full_dict[size_dict++] = x[j] ? strdup(delim_word->p2) : strdup(delim_word->p1);
          }
          push_queue(global_queue, full_dict, cur);
        }
        for (size_t i = 0; i < delim; i++)
        {
          free(delim_words[i].p1);
          free(delim_words[i].p2);
        }
        FREE_PP(auxPerm, 0, cur);
      }
      else
      {
        push_queue(global_queue, auxPerm, cur);
      }
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
  while ((c = getopt_long(argc, argv, "u:l:pk:c:s:e:r:", long_options, &option_index)) != -1)
  {
    switch (c)
    {
    case 'r':
    {
      CALL_ZERO_SET_TRUE(strcmp(optarg, "full"), bool_modifiers.reverse_full);
      CALL_ZERO_SET_TRUE(strcmp(optarg, "words"), bool_modifiers.reverse_words);
      break;
    }
    case 'k':
    {
      CALL_ZERO_SET_TRUE(strcmp(optarg, "full"), bool_modifiers.leet_full);
      CALL_ZERO_SET_TRUE(strcmp(optarg, "vowel"), bool_modifiers.leet_vowel);
      break;
    }
    case 'u':
    {
      CALL_ZERO_SET_TRUE(strcmp(optarg, "full"), bool_modifiers.upper_full);
      CALL_ZERO_SET_TRUE(strcmp(optarg, "first"), bool_modifiers.upper_first);
      break;
    }
    case 'p':
    {
      CALL_ZERO_SET_TRUE(0, bool_modifiers.only_transform);
      break;
    }
    case 'c':
    {
      connectors = (char **)malloc(sizeof(char *) * BUFF);
      char connector_to_copy[2] = {0x0};
      connectors_size = strlen(optarg);
      for (size_t i = 0; i < connectors_size; i++)
      {
        connector_to_copy[0] = optarg[i];
        connectors[i] = (char *)malloc(sizeof(char) * 2);
        memccpy(connectors[i], connector_to_copy, '\0', sizeof(char) * 2);
      }
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
        last[x++][strlen(token)] = '\0';
        token = strtok(NULL, ",");
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
      CHECK_TRUE(true, "wrong parameters");
      break;
    }
    default:
    {
      CHECK_TRUE(true, "optarg failure");
    }
    }
  }

  CHECK_TRUE(!min_len, "max_len must be stated");
  CHECK_TRUE(!max_len, "min_len must be stated");
  LOW("max_len must be greater than min_len", max_len, min_len);
  CHECK_TRUE(optind == argc, "words not provided");

  leet_map['a'] = leet_map['A'] = '4';
  leet_map['e'] = leet_map['E'] = '3';
  leet_map['i'] = leet_map['I'] = '1';
  leet_map['o'] = leet_map['O'] = '0';
  if (bool_modifiers.leet_full)
  {
    leet_map['s'] = leet_map['S'] = '5';
    leet_map['t'] = leet_map['T'] = '7';
    leet_map['g'] = leet_map['G'] = '9';
    leet_map['z'] = leet_map['Z'] = '2';
  }

  word_size = (size_t)argc - (size_t)optind;
  global_queue = default_init_queue();
  size_t n_delim = 0;
  size_t optind_copy = optind;
  for (size_t i = 0; i < word_size; i++)
  {
    if (strchr(argv[optind_copy++], ',') != NULL)
    {
      n_delim++;
    }
  }
  unsigned short ***bin_delim = (unsigned short ***)malloc(sizeof(unsigned short **) * (n_delim + 1));
  for (size_t i = 1; i < n_delim + 1; i++)
  {
    bin_delim[i] = (unsigned short **)malloc(sizeof(unsigned short *) * (1 << i));
    unsigned short *bin = (unsigned short *)calloc(i, sizeof(unsigned short));
    size_t bd_size = 0;
    gen_bin_to_arr(bin, i, 0, i, 0, 0, bin_delim, &bd_size);
    FREE_P(bin);
  }
  delim_bins = bin_delim;
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
  FREE_P(bin);
  pthread_t tworker[N_THREAD];
  memset(&tworker, 0x0, sizeof(tworker));
  size_t pos = 0;
  for (size_t i = 0; i < N_THREAD; i++)
  {
    pthread_create(&tworker[i], NULL, thread_perm, NULL);
  }
  for (size_t i = 0; i < N_THREAD; i++)
  {
    pthread_join(tworker[i], NULL);
  }
  free_queue(global_queue);
  FREE_P(global_queue);
  FREE_P(input_words);
  FREE_PP(connectors, 0, connectors_size);
  FREE_PP(last, 0, last_size);
  FREE_PPP(bin_delim, 1, n_delim + 1, 0, (1 << i));
  return 0;
}
