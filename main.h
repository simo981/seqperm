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

void printOut(char** arr, size_t size);
void swapPP(char **f, char **s);
void seqPerm(char **arr, size_t size);
void *threadPerm(void *in);
void genBin(unsigned short *arr, size_t s, size_t occ);
void exitUsage(char *plus);

void exitUsage(char *plus) {
  printf("%s\n./wlgen -s <minLen> -e <maxLen> chars/words\n",
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
    exitUsage(#NAME);                                                          \
  }

#define lt0(NAME, VAR)                                                         \
  if (VAR < 0) {                                                               \
    exitUsage(#NAME);                                                          \
  }

#define exErr(NAME)                                                            \
  perror(#NAME);                                                               \
  exit(EXIT_FAILURE);

#endif
