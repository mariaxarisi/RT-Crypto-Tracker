#include "../include/average.h"
#include "../include/vector.h"
#include "../include/file.h"

#define SYMBOL_COUNT 8

extern volatile int interrupted;
extern const char *symbols[];
extern TradeVector **trades;

// Get current timestamp in milliseconds
long long current_timestamp_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (long long)(ts.tv_sec) * 1000 + ts.tv_nsec / 1000000;
}

// Calculate moving average from file
void calculate_moving_average(int i, long long window_end) {

    long long window_start = window_end - 15 * 60 * 1000;

    double sum = 0.0;
    double volume = 0.0;

    for(int j=0; j<(int)trades[i]->size; j++){
        Trade trade = trade_vector_get(trades[i], j);
        if (trade.ts < window_start) {
            trade_vector_erase(trades[i], j);
        }
        else if(trade.ts <= window_end && trade.ts >= window_start){
            sum += trade.px * trade.sz;
            volume += trade.sz;
        }
        else{
            break;
        }
    }

    if (volume > 0.0) {
        double average = sum / volume;
        write_average(symbols[i], window_end, average);
    }
}

// The thread function
void* average_thread_func(void *arg) {
    (void)arg;
    
    long long base_time = current_timestamp_ms();
    // Round base_time to the next full minute
    base_time = base_time - (base_time % 60000) + 60000;

    while (!interrupted) {
        long long now = current_timestamp_ms();

        if (now < base_time) {
            usleep((base_time - now) * 1000);
        }

        long long start = base_time;

        for (int i = 0; i < SYMBOL_COUNT; i++) {
            calculate_moving_average(i, start);
        }

        base_time += 60000; // Schedule for the next exact minute
    }

    return NULL;
}