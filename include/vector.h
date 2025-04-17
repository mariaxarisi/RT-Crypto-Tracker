#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stddef.h>

typedef struct Trade{
    long long ts;
    double px;
    double sz;
} Trade;

typedef struct TradeVector {
    Trade *trades;
    size_t capacity;
    size_t size;

    pthread_mutex_t mutex;
} TradeVector;

typedef struct TradeVector TradeVector;
TradeVector *trade_vector_create(size_t capacity);
void trade_vector_destroy(TradeVector *vector);
void trade_vector_push(TradeVector *vector, long long ts, double px, double sz);
Trade trade_vector_erase(TradeVector *vector, size_t index);
Trade trade_vector_get(TradeVector *vector, size_t index);

#endif // VECTOR_H