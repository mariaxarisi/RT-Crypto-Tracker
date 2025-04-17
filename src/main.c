#include <pthread.h>
#include <signal.h>
#include "../include/queue.h"
#include "../include/lws.h"
#include "../include/file.h"
#include "../include/parser.h"

volatile int interrupted = 0;
Queue *queue = NULL;

void sigint_handler(int sig) {
    printf("\n[Main] Signal %d received, stopping...\n", sig);
    interrupted = 1;
}

int main(void) {
    signal(SIGINT, sigint_handler);

    struct lws_context *context = NULL;
    struct lws *wsi = NULL;

    create_files();
    queue = queue_create(1000);

    if (websocket_start(&context, &wsi) != 0) {
        return 1;
    }

    pthread_t ws_thread, parser_thread;
    pthread_create(&ws_thread, NULL, websocket_thread_func, context);
    pthread_create(&parser_thread, NULL, parse_thread_func, queue);

    // Wait for threads to finish
    pthread_join(ws_thread, NULL);
    queue_push(queue, NULL); // Signal the parser thread to stop
    pthread_join(parser_thread, NULL);

    websocket_stop(context);
    queue_destroy(queue);

    return 0;
}