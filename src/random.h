#pragma once
#include <unistd.h>

enum c_t
{
    ASCII = 0,
    NUM,
    ALPHA,
    ALPHALOWER,
    ALPHAUPPER,
    ALPHANUM,
    ALPHANUMUPPER,
    USER,
    _NULL
};

void main_random(size_t times, size_t length, enum c_t type, char *charset, size_t N_THREAD_);
void *thread_random(void *in);
void rand_gen(unsigned int *seed, char *buff, enum c_t type, size_t n);
