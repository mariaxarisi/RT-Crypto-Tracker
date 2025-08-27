#include <pthread.h>
#include <signal.h>
#include <time.h>
#include "queue.h"
#include "vector.h"
#include "file.h"
#include "buffer.h"
#include "lws.h"
#include "parser.h"
#include "average.h"
#include "pearson.h"

const int SYMBOL_COUNT = 8;
const int BUFFER_SIZE = 67;
const int ONE_MINUTE_MS = 60000;
volatile int interrupted = 0;
MessageQueue *message_queue = NULL;
TradeVector **trades = NULL;
MvAvgBuffer **mv_buf = NULL;
struct lws_context *context = NULL;
struct lws *wsi = NULL;

const char *symbols[] = {
    "BTC-USDT", "ADA-USDT", "ETH-USDT", "DOGE-USDT",
    "XRP-USDT", "SOL-USDT", "LTC-USDT", "BNB-USDT"
};

long long current_timestamp_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (long long)(ts.tv_sec) * 1000 + ts.tv_nsec / 1000000;
}

void sigint_handler(int sig) {
    printf("\n[Main] Signal %d received, stopping...\n", sig);
    interrupted = 1;
}

int main(void) {
    signal(SIGINT, sigint_handler);

    create_files();
    message_queue = message_queue_create(200);
    trades = malloc(sizeof(TradeVector *) * SYMBOL_COUNT);
    for (int i = 0; i < SYMBOL_COUNT; ++i) {
        trades[i] = trade_vector_create(500);
    }
    mv_buf = malloc(sizeof(MvAvgBuffer *) * SYMBOL_COUNT);
    for (int i = 0; i < SYMBOL_COUNT; ++i) {
        mv_buf[i] = mvavg_buffer_create(BUFFER_SIZE);
    }

    if (websocket_start(&context, &wsi) != 0) {
        return 1;
    }

    pthread_t ws_thread, parser_thread, average_thread, pearson_thread;
    pthread_create(&ws_thread, NULL, websocket_thread_func, NULL);
    pthread_create(&parser_thread, NULL, parse_thread_func, message_queue);
    long long mv_start = current_timestamp_ms();
    pthread_create(&average_thread, NULL, average_thread_func, &mv_start);
    pthread_create(&pearson_thread, NULL, pearson_thread_func, &mv_start);

    // Wait for threads to finish
    pthread_join(ws_thread, NULL);
    message_queue_push(message_queue, NULL); // Signal the parser thread to stop
    pthread_join(parser_thread, NULL);
    pthread_join(average_thread, NULL);
    pthread_join(pearson_thread, NULL);

    message_queue_destroy(message_queue);
    for (int i = 0; i < SYMBOL_COUNT; ++i) {
        trade_vector_destroy(trades[i]);
    }
    free(trades);
    for (int i = 0; i < SYMBOL_COUNT; ++i) {
        mvavg_buffer_destroy(mv_buf[i]);
    }
    free(mv_buf);

    return 0;
}