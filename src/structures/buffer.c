#include "buffer.h"

MvAvgBuffer* mvavg_buffer_create(int size) {
    MvAvgBuffer* mvavg_buf = malloc(sizeof(MvAvgBuffer));
    if (!mvavg_buf) {
        fprintf(stderr, "Failed to allocate memory for MvAvgBuffer\n");
        return NULL;
    }

    mvavg_buf->buffer = malloc(size * sizeof(MvAverage));
    if (!mvavg_buf->buffer) {
        fprintf(stderr, "Failed to allocate memory for MvAverage buffer\n");
        free(mvavg_buf);
        return NULL;
    }
    
    mvavg_buf->max_size = size;
    mvavg_buf->count = 0;
    mvavg_buf->start = 0;
    pthread_mutex_init(&mvavg_buf->mutex, NULL);
    
    return mvavg_buf;
}

void mvavg_buffer_destroy(MvAvgBuffer* mvavg_buf) {
    if (mvavg_buf) {
        free(mvavg_buf->buffer);
        pthread_mutex_destroy(&mvavg_buf->mutex);
        free(mvavg_buf);
    }
}

int mvavg_buffer_push(MvAvgBuffer* mvavg_buf, long long ts, double value) {
    pthread_mutex_lock(&mvavg_buf->mutex);

    int end = (mvavg_buf->start + mvavg_buf->count) % mvavg_buf->max_size;
    mvavg_buf->buffer[end].ts = ts;
    mvavg_buf->buffer[end].value = value;
    
    if(mvavg_buf->count == mvavg_buf->max_size) {
        mvavg_buf->start = (mvavg_buf->start + 1) % mvavg_buf->max_size;
    }else{
        mvavg_buf->count++;
    }

    pthread_mutex_unlock(&mvavg_buf->mutex);
    
    return 0;
}

MvAverage mvavg_buffer_get(MvAvgBuffer* mvavg_buf, int index) {
    pthread_mutex_lock(&mvavg_buf->mutex);
    
    if (index < 0 || index >= mvavg_buf->count) {
        fprintf(stderr, "Index out of bounds\n");
        pthread_mutex_unlock(&mvavg_buf->mutex);
        MvAverage invalid_mv = {0, 0.0};
        return invalid_mv;
    }
    
    int actual_index = (mvavg_buf->start + index) % mvavg_buf->max_size;
    MvAverage result = mvavg_buf->buffer[actual_index];
    
    pthread_mutex_unlock(&mvavg_buf->mutex);
    
    return result;
}
