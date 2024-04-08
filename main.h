#ifndef MAIN_H
#define MAIN_H
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define BUFF 512

void print_out(char **arr, size_t size);
void print_f_maiusc(char **arr, char *string);
void print_number(char **arr, char *finalString, size_t run_len);
void swap_p(char **f, char **s);
void seq_perm(char **arr, size_t size);
void *thread_perm(void *in);
void gen_bin_perms(unsigned short *arr, size_t size, size_t idx, size_t max, size_t cur, size_t min);
size_t add_string(char *buff[BUFF], size_t idx, char *to_push, size_t to_push_len);
void free_inputs_optind(void);
void exit_usage(char *plus);
unsigned leet_encode(char *str);

void exit_usage(char *plus)
{
  printf("%s\n./seqperm --upper (y/n) --allupper (y/n) --merged (y/n) --leet (full/vowel) --start <min words> --end <max words> --last N1,N2,... --connectors ... w1 w2 w3 w4\n", plus);
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

#define UNDEF(NAME, VAR)    \
  if (VAR == NULL)           \
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

#define LOW(NAME, VAR1, VAR2)    \
  if (VAR1 < VAR2)           \
  {                       \
    perror(#NAME);        \
    free_inputs_optind(); \
    exit(EXIT_FAILURE);   \
  }  

#define exErr(NAME) \
  perror(#NAME);    \
  exit(EXIT_FAILURE);

#endif
