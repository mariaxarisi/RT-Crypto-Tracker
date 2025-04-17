#include "../include/parser.h"
#include "../include/file.h"
#include "../include/queue.h"
#include "../include/vector.h"

extern volatile int interrupted;
extern const char *symbols[];
extern TradeVector **trades;

void* parse_thread_func(void *arg) {
    MessageQueue *message_queue = (MessageQueue *)arg;
    char *message;

    while (1) {
        message = message_queue_pop(message_queue);
        if (message == NULL) {
            break;
        }

        parse_trade(message);
        free(message);
    }

    return NULL;
}

void parse_trade(const char *json_msg) {
    json_t *root, *data, *trade, *px, *sz, *ts, *arg, *instId;
    json_error_t error;

    root = json_loads(json_msg, 0, &error);
    if (!root){
        fprintf(stderr, "Error parsing JSON: %s\n", error.text);
        return;
    }

    data = json_object_get(root, "data");
    arg = json_object_get(root, "arg");
    instId = json_object_get(arg, "instId");

    if (!json_is_array(data) || !json_is_string(instId)) {
        fprintf(stderr, "Invalid JSON structure: %s\n", json_msg);
        json_decref(root);
        return;
    }

    for (size_t i = 0; i < json_array_size(data); ++i) {
        trade = json_array_get(data, i);
        px = json_object_get(trade, "px");
        sz = json_object_get(trade, "sz");
        ts = json_object_get(trade, "ts");

        if (json_is_string(px) && json_is_string(sz) && json_is_string(ts)) {
            write_trade(json_string_value(instId), json_string_value(ts),
                        json_string_value(px), json_string_value(sz));

            int i = 0;
            for (i = 0; i < 8; ++i) {
                if (strcmp(json_string_value(instId), symbols[i]) == 0) {
                    break;
                }
            }

            // Push trade data to the trade queue
            long long timestamp = strtoll(json_string_value(ts), NULL, 10);
            double price = strtod(json_string_value(px), NULL);
            double size = strtod(json_string_value(sz), NULL);
            trade_vector_push(trades[i], timestamp, price, size);

        }else {
            fprintf(stderr, "Invalid trade data: %s\n", json_msg);
        }
    }

    json_decref(root);
}