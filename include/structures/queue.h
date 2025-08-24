#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stddef.h>

typedef struct {
    char **messages;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t size;

    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
} MessageQueue;

MessageQueue *message_queue_create(size_t capacity);
void message_queue_destroy(MessageQueue *queue);
void message_queue_push(MessageQueue *queue, char *message);
char *message_queue_pop(MessageQueue *queue);

#endif // QUEUE_H
