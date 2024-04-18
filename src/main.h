#ifndef MAIN_H
#define MAIN_H
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define BUFF 512
#define N_THREAD 8

#define FREE_PPP(p, fc, size, sc, size2) \
  ({                                     \
    for (size_t i = fc; i < size; i++)   \
    {                                    \
      FREE_PP(p[i], sc, size2);          \
    }                                    \
    FREE_P(p);                           \
  })

#define FREE_PP(p, fc, size)             \
  ({                                     \
    if (p)                               \
    {                                    \
      for (size_t j = fc; j < size; j++) \
      {                                  \
        FREE_P(p[j]);                    \
      }                                  \
      FREE_P(p);                         \
    }                                    \
  })

#define FREE_P(p) \
  ({              \
    if (p)        \
    {             \
      free(p);    \
    }             \
  })

#define COPY_STRING(dest, src) ({     \
  size_t copy_len = strlen(src);      \
  memccpy(dest, src, '\0', copy_len); \
  dest[copy_len] = '\0';              \
  copy_len;                           \
})

#define CALL_ZERO_SET_TRUE(call, var) \
  ({                                  \
    if (call == 0)                    \
    {                                 \
      var = true;                     \
    }                                 \
  })

#define CHECK_TRUE(var, message) \
  ({                             \
    if (var)                     \
    {                            \
      free_inputs_optind();      \
      exit_usage(#message);      \
    }                            \
  })

typedef struct bool_t
{
  uint8_t leet_vowel : 1;
  uint8_t leet_full : 1;
  uint8_t upper_first : 1;
  uint8_t upper_full : 1;
  uint8_t only_transform : 1;
  uint8_t reverse_words : 1;
  uint8_t reverse_full : 1;
  uint8_t __padding : 1;
} modifiers_t;

typedef struct delim_t
{
  char *p1;
  char *p2;
} delim_t;

void print_out(char **arr, size_t size);
void print_f_maiusc(char **arr, char *string);
void print_number(char **arr, char *finalString, size_t run_len);
void swap_p(char **f, char **s);
void seq_perm(char **arr, size_t size);
void *thread_perm(void *in);
void gen_bin_perms(unsigned short *arr, size_t size, size_t idx, size_t max, size_t cur, size_t min);
size_t add_string(char *buff[BUFF], size_t idx, char *to_push, size_t to_push_len);
void gen_bin_to_arr(unsigned short *arr, size_t size, size_t idx, size_t max, size_t cur, size_t min, unsigned short ***out, size_t *out_size);
size_t add_string_no_copy(char *buff[BUFF], size_t idx, char *to_push);
unsigned **binomial_coefficient(size_t n, size_t k);
void free_inputs_optind(void);
void exit_usage(char *plus);
bool leet_encode(char *str);
bool upper_encode(char *str);
void reverse(char *str, size_t len);
bool palindrome(char *str, size_t len);

void exit_usage(char *plus)
{
  printf("%s\n./seqperm --upper full/first --leet full/vowel --only_transformations y/n --start <min words> --end <max words> --last N1,N2,... --connectors ... w1 w2 w3 w4\n", plus);
  exit(EXIT_FAILURE);
}

#define ERR(NAME, VAR, CALL) \
  errno = 0;                 \
  VAR = CALL;                \
  if (errno != 0)            \
  {                          \
    perror(#NAME);           \
    free_inputs_optind();    \
    exit(EXIT_FAILURE);      \
  }

#define UNDEF(NAME, VAR)  \
  if (VAR == NULL)        \
  {                       \
    perror(#NAME);        \
    free_inputs_optind(); \
    exit(EXIT_FAILURE);   \
  }

#define LE0(NAME, VAR)    \
  if (VAR <= 0)           \
  {                       \
    perror(#NAME);        \
    free_inputs_optind(); \
    exit(EXIT_FAILURE);   \
  }

#define LOW(NAME, VAR1, VAR2) \
  if (VAR1 < VAR2)            \
  {                           \
    free_inputs_optind();     \
    exit_usage(#NAME);        \
  }

#define exErr(NAME) \
  perror(#NAME);    \
  exit(EXIT_FAILURE);

#endif
