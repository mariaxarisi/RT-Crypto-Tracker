#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct MvAverage{
    long long ts;
    double value;
} MvAverage;

typedef struct MvAvgBuffer{
    MvAverage* buffer;
    int max_size;
    int count;
    int start;
    pthread_mutex_t mutex;
} MvAvgBuffer;

MvAvgBuffer* mvavg_buffer_create(int size);
void mvavg_buffer_destroy(MvAvgBuffer* mvavg_buf);
int mvavg_buffer_push(MvAvgBuffer* mvavg_buf, long long ts, double value);
MvAverage mvavg_buffer_get(MvAvgBuffer* mvavg_buf, int index);

#endif // BUFFER_H