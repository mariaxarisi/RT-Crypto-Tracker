#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>

typedef struct Queue Queue;
Queue *queue_create(size_t capacity);
void queue_destroy(Queue *queue);
void queue_push(Queue *queue, char *message);
char *queue_pop(Queue *queue);

#endif // QUEUE_H
