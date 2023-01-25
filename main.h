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

void print_out(char **arr, size_t size);
void swap_p(char **f, char **s);
void seq_perm(char **arr, size_t size);
void *thread_perm(void *in);
void gen_bin(unsigned short *arr, size_t s, size_t occ);
void exit_usage(char *plus);

void exit_usage(char *plus) {
  printf("%s\n./wlgen [-n]NoNumberAtStart [-f]FirstLetterUpperCase -s <MinLen> "
         "-e <MaxLen> chars/words/numbers\n",
         plus);
  exit(EXIT_FAILURE);
}

#define err(NAME, VAR, CALL)                                                   \
  errno = 0;                                                                   \
  VAR = CALL;                                                                  \
  if (errno != 0) {                                                            \
    perror(#NAME);                                                             \
    exit(EXIT_FAILURE);                                                        \
  }

#define le0(NAME, VAR)                                                         \
  if (VAR <= 0) {                                                              \
    exit_usage(#NAME);                                                         \
  }

#define lt0(NAME, VAR)                                                         \
  if (VAR < 0) {                                                               \
    exit_usage(#NAME);                                                         \
  }

#define exErr(NAME)                                                            \
  perror(#NAME);                                                               \
  exit(EXIT_FAILURE);

#endif
