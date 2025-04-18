#ifndef PEARSON_H
#define PEARSON_H

#include <stdio.h>
#include <stdlib.h> 
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include "buffer.h"
#include "file.h"

typedef struct maxCorr {
    long long ts;
    double value;
} maxCorr;

void* pearson_thread_func(void *arg);
maxCorr max_pearson_correlation(mv_buffer *mv_buf1, mv_buffer *mv_buf2);
double calculate_pearson_correlation(double *x, double *y, int n);

#endif // PEARSON_H
