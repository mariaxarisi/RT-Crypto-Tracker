#include "../../include/structures/queue.h"

struct MessageQueue {
    char **messages;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t size;

    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
};

MessageQueue *message_queue_create(size_t capacity) {
    MessageQueue *q = malloc(sizeof(MessageQueue));
    q->messages = malloc(sizeof(char *) * capacity);
    q->capacity = capacity;
    q->head = q->tail = q->size = 0;

    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_empty, NULL);

    return q;
}

void message_queue_destroy(MessageQueue *q) {
    if (!q) return;

    pthread_mutex_lock(&q->mutex);
    for (size_t i = 0; i < q->size; ++i) {
        free(q->messages[(q->head + i) % q->capacity]);
    }
    pthread_mutex_unlock(&q->mutex);

    free(q->messages);
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->not_empty);
    free(q);
}

void message_queue_push(MessageQueue *q, char *message) {
    pthread_mutex_lock(&q->mutex);
 
    // Resize if needed
    if (q->size == q->capacity) {
        size_t new_capacity = q->capacity * 2;
        char **new_messages = malloc(sizeof(char *) * new_capacity);
        for (size_t i = 0; i < q->size; ++i) {
            new_messages[i] = q->messages[(q->head + i) % q->capacity];
        }
        free(q->messages);
        q->messages = new_messages;
        q->capacity = new_capacity;
        q->head = 0;
        q->tail = q->size;
    }

    q->messages[q->tail] = message;
    q->tail = (q->tail + 1) % q->capacity;
    q->size++;

    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
}

char *message_queue_pop(MessageQueue *q) {
    pthread_mutex_lock(&q->mutex);
    while (q->size == 0) {
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }

    char *msg = q->messages[q->head];
    q->head = (q->head + 1) % q->capacity;
    q->size--;

    pthread_mutex_unlock(&q->mutex);
    return msg;
}