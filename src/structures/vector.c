#include "vector.h"

TradeVector *trade_vector_create(size_t capacity) {
    TradeVector *vector = malloc(sizeof(TradeVector));
    if (!vector) {
        fprintf(stderr, "Failed to allocate memory for TradeVector\n");
        exit(EXIT_FAILURE);
    }
    vector->trades = malloc(sizeof(Trade) * capacity);
    if (!vector->trades) {
        fprintf(stderr, "Failed to allocate memory for TradeVector trades\n");
        exit(EXIT_FAILURE);
    }
    vector->capacity = capacity;
    vector->size = 0;

    pthread_mutex_init(&vector->mutex, NULL);

    return vector;
}

void trade_vector_destroy(TradeVector *vector) {
    if (!vector) return;

    pthread_mutex_lock(&vector->mutex);
    free(vector->trades);
    pthread_mutex_unlock(&vector->mutex);

    pthread_mutex_destroy(&vector->mutex);
    free(vector);
}

void trade_vector_push(TradeVector *vector, long long ts, double px, double sz) {
    pthread_mutex_lock(&vector->mutex);

    // Resize if needed
    if (vector->size == vector->capacity) {
        size_t new_capacity = vector->capacity * 2;
        Trade *new_trades = realloc(vector->trades, sizeof(Trade) * new_capacity);
        if (!new_trades) {
            pthread_mutex_unlock(&vector->mutex);
            fprintf(stderr, "Failed to resize TradeVector\n");
            exit(EXIT_FAILURE);
        }
        vector->trades = new_trades;
        vector->capacity = new_capacity;
    }

    vector->trades[vector->size].ts = ts;
    vector->trades[vector->size].px = px;
    vector->trades[vector->size].sz = sz;
    vector->size++;

    pthread_mutex_unlock(&vector->mutex);
}

Trade trade_vector_erase(TradeVector *vector, size_t index) {
    pthread_mutex_lock(&vector->mutex);

    if (index >= vector->size) {
        pthread_mutex_unlock(&vector->mutex);
        fprintf(stderr, "Index out of bounds\n");
        exit(EXIT_FAILURE);
    }

    Trade trade = vector->trades[index];
    for (size_t i = index; i < vector->size - 1; ++i) {
        vector->trades[i] = vector->trades[i + 1];
    }
    vector->size--;

    pthread_mutex_unlock(&vector->mutex);
    return trade;
}

Trade trade_vector_get(TradeVector *vector, size_t index) {
    pthread_mutex_lock(&vector->mutex);

    if (index >= vector->size) {
        pthread_mutex_unlock(&vector->mutex);
        fprintf(stderr, "Index out of bounds\n");
        exit(EXIT_FAILURE);
    }

    Trade trade = vector->trades[index];

    pthread_mutex_unlock(&vector->mutex);
    return trade;
}