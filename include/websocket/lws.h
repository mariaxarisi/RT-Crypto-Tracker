#ifndef LWS_H
#define LWS_H

#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include <jansson.h>
#include "../structures/queue.h"

typedef struct WsThreadArgs WsThreadArgs;
void *websocket_thread_func(void *arg);
int callback_ws(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);
extern struct lws_protocols protocols[];
int websocket_start(struct lws_context **ctx_out, struct lws **wsi_out);
void websocket_stop(struct lws_context *context);

#endif // LWS_H