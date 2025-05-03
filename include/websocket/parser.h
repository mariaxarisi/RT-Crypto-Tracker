#ifndef PARSER_H
#define PARSER_H

#include <jansson.h>
#include "../structures/queue.h"
#include "../structures/vector.h"
#include "../structures/file.h"

void* parse_thread_func(void *arg);
void parse_trade(const char *json_msg);

#endif // PARSER_H