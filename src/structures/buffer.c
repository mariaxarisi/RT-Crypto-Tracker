#include <../../include/structures/buffer.h>

mv_buffer* create_mv_buffer(int size) {
    mv_buffer* mv_buf = (mv_buffer*)malloc(sizeof(mv_buffer));
    if (!mv_buf) {
        fprintf(stderr, "Failed to allocate memory for mv_buffer\n");
        return NULL;
    }
    
    mv_buf->buffer = (mv*)malloc(size * sizeof(mv));
    if (!mv_buf->buffer) {
        fprintf(stderr, "Failed to allocate memory for buffer\n");
        free(mv_buf);
        return NULL;
    }
    
    mv_buf->max_size = size;
    mv_buf->count = 0;
    mv_buf->start = 0;
    pthread_mutex_init(&mv_buf->mutex, NULL);
    
    return mv_buf;
}

void destroy_mv_buffer(mv_buffer* mv_buf) {
    if (mv_buf) {
        free(mv_buf->buffer);
        pthread_mutex_destroy(&mv_buf->mutex);
        free(mv_buf);
    }
}

int mv_buffer_push(mv_buffer* mv_buf, long long ts, double value) {
    pthread_mutex_lock(&mv_buf->mutex);

    int end = (mv_buf->start + mv_buf->count) % mv_buf->max_size;
    mv_buf->buffer[end].ts = ts;
    mv_buf->buffer[end].value = value;
    
    if(mv_buf->count == mv_buf->max_size) {
        mv_buf->start = (mv_buf->start + 1) % mv_buf->max_size;
    }else{
        mv_buf->count++;
    }

    pthread_mutex_unlock(&mv_buf->mutex);
    
    return 0;
}

mv mv_buffer_get(mv_buffer* mv_buf, int index) {
    pthread_mutex_lock(&mv_buf->mutex);
    
    if (index < 0 || index >= mv_buf->count) {
        fprintf(stderr, "Index out of bounds\n");
        pthread_mutex_unlock(&mv_buf->mutex);
        mv invalid_mv = {0, 0.0};
        return invalid_mv;
    }
    
    int actual_index = (mv_buf->start + index) % mv_buf->max_size;
    mv result = mv_buf->buffer[actual_index];
    
    pthread_mutex_unlock(&mv_buf->mutex);
    
    return result;
}
