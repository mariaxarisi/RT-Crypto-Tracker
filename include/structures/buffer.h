#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct mv{
    long long ts;
    double value;
} mv;

typedef struct mv_buffer{
    mv* buffer;
    int max_size;
    int count;
    int start;
    pthread_mutex_t mutex;
} mv_buffer;

mv_buffer* create_mv_buffer(int size);
void destroy_mv_buffer(mv_buffer* mv_buf);
int mv_buffer_push(mv_buffer* mv_buf, long long ts, double value);
mv mv_buffer_get(mv_buffer* mv_buf, int index);

#endif // BUFFER_H