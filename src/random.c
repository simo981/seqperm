#include "random.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdatomic.h>

char *ascii = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";
char *dataset;
const size_t se[8][2] = {{0, 95}, {0, 10}, {10, 62}, {36, 62}, {10, 36}, {0, 62}, {0, 36}};

typedef struct random_t
{
    size_t size;
    size_t times;
    enum c_t type;
} random_t;

void *thread_random(void *in)
{
    random_t *x = (random_t *)in;
    unsigned int myseed = time(NULL) ^ (size_t)pthread_self();
    char *out = calloc(x->size + 1, sizeof(char));
    size_t start, end;
    start = x->type != USER ? se[x->type][0] : 0;
    end = x->type != USER ? se[x->type][1] : strlen(dataset);
    for (size_t i = 0; i < x->times; i++)
    {
        for (size_t j = 0; j < x->size; j++)
        {
            out[j] = dataset[(rand_r(&myseed) % (end - start)) + start];
        }
        printf("%s\n", out);
    }
    free(x);
    free(out);
    return NULL;
}

void main_random(size_t times, size_t length, enum c_t type, char *charset, size_t N_THREAD_)
{
    dataset = ascii;
    dataset = type == USER && charset != NULL ? charset : dataset;
    pthread_t tworker[N_THREAD_];
    memset(&tworker, 0x0, sizeof(tworker));
    size_t ttimes = times / N_THREAD_;
    size_t mod = times % N_THREAD_;
    for (size_t i = 0; i < N_THREAD_; i++)
    {
        random_t *x = (random_t *)malloc(sizeof(random_t));
        x->size = length;
        x->type = type;
        x->times = i == 0 ? ttimes + mod : ttimes;
        pthread_create(&tworker[i], NULL, thread_random, (void *)x);
    }
    for (size_t i = 0; i < N_THREAD_; i++)
    {
        pthread_join(tworker[i], NULL);
    }
    return;
}