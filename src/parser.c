#include "../include/parser.h"
#include "../include/file.h"
#include "../include/queue.h"

extern volatile int interrupted;

void* parse_thread_func(void *arg) {
    Queue *queue = (Queue *)arg;
    char *message;

    while (!interrupted) {
        message = queue_pop(queue);
        if (message) {
            parse_trade(message);
            free(message);
        }
    }

    return NULL;
}

void parse_trade(const char *json_msg) {
    json_t *root, *data, *trade, *px, *sz, *ts, *arg, *instId;
    json_error_t error;

    root = json_loads(json_msg, 0, &error);
    if (!root) return;

    data = json_object_get(root, "data");
    arg = json_object_get(root, "arg");
    instId = json_object_get(arg, "instId");

    if (!json_is_array(data) || !json_is_string(instId)) {
        json_decref(root);
        return;
    }

    for (size_t i = 0; i < json_array_size(data); ++i) {
        trade = json_array_get(data, i);
        px = json_object_get(trade, "px");
        sz = json_object_get(trade, "sz");
        ts = json_object_get(trade, "ts");

        if (json_is_string(px) && json_is_string(sz) && json_is_string(ts)) {
            write_trade(json_string_value(instId),
                        json_string_value(ts),
                        json_string_value(px),
                        json_string_value(sz));
        }
    }

    json_decref(root);
}