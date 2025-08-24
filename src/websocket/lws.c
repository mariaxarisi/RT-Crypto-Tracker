#include "lws.h"

extern MessageQueue *message_queue;
extern volatile int interrupted;
extern struct lws_context *context;
extern struct lws *wsi;
extern const char *symbols[];
volatile int reconnect = 0;

struct lws_protocols protocols[] = {
    {
        .name = "okx-protocol",
        .callback = callback_ws,
        .per_session_data_size = 0,
    },
    { NULL, NULL, 0, 0 }
};

void *websocket_thread_func(void *arg) {

    while (!interrupted) {
        if(reconnect || context == NULL || wsi == NULL) {
            if(context) {
                websocket_stop(context);
                context = NULL;
                wsi = NULL;
            }

            printf("[WS] Attempting to reconnect...\n");
            if (websocket_start(&context, &wsi) != 0) {
                printf("WebSocket restart failed, retrying in 2 seconds...\n");
                usleep(2000000);
                continue;
            }
            reconnect = 0;
        }

        lws_service(context, 100);
    }

    if(context) {
        websocket_stop(context);
    }

    return NULL;
}

// WebSocket protocol callback
int callback_ws(struct lws *wsi, enum lws_callback_reasons reason,
                       void *user, void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            printf("[WS] Connected to OKX\n");

            // Create JSON subscription message
            json_t *root = json_object();
            json_object_set_new(root, "op", json_string("subscribe"));

            json_t *args = json_array();

            for (int i = 0; i < 8; ++i) {
                json_t *arg = json_object();
                json_object_set_new(arg, "channel", json_string("trades"));
                json_object_set_new(arg, "instId", json_string(symbols[i]));
                json_array_append_new(args, arg);
            }
            json_object_set_new(root, "args", args);

            char *json_str = json_dumps(root, 0);
            size_t msg_len = strlen(json_str);

            unsigned char *buf = malloc(LWS_PRE + msg_len);
            memcpy(buf + LWS_PRE, json_str, msg_len);

            lws_write(wsi, buf + LWS_PRE, msg_len, LWS_WRITE_TEXT);

            free(json_str);
            free(buf);
            json_decref(root);
            break;

        case LWS_CALLBACK_CLIENT_RECEIVE:
            //printf("[WS] Received message: %s\n", (char *)in);
            char *message = malloc(len + 1);
            memcpy(message, in, len);
            message[len] = '\0';
            message_queue_push(message_queue, message);
            break;

        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            printf("[WS] Connection error\n");
            reconnect = 1;
            break;

        case LWS_CALLBACK_CLIENT_CLOSED:
            printf("[WS] Connection closed\n");
            reconnect = 1;
            break;

        default:
            break;
    }
    return 0;
}

int websocket_start(struct lws_context **ctx_out, struct lws **wsi_out) {
    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(info));
    info.port = CONTEXT_PORT_NO_LISTEN;
    info.protocols = protocols;
    info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;

    *ctx_out = lws_create_context(&info);
    if (!*ctx_out) {
        fprintf(stderr, "Failed to create LWS context\n");
        return 1;
    }

    struct lws_client_connect_info ccinfo = {0};
    ccinfo.context = *ctx_out;
    ccinfo.address = "ws.okx.com";
    ccinfo.port = 8443;
    ccinfo.path = "/ws/v5/public";
    ccinfo.host = ccinfo.address;
    ccinfo.origin = "https://ws.okx.com";
    ccinfo.ssl_connection = LCCSCF_USE_SSL;
    ccinfo.protocol = protocols[0].name;

    *wsi_out = lws_client_connect_via_info(&ccinfo);
    if (!*wsi_out) {
        fprintf(stderr, "WebSocket connection failed\n");
        lws_context_destroy(*ctx_out);
        return 1;
    }

    return 0;
}

void websocket_stop(struct lws_context *context) {
    if (context) {
        lws_context_destroy(context);
    }
}