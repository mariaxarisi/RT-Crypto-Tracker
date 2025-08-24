#ifndef PARSER_H
#define PARSER_H

#include <jansson.h>
#include "queue.h"
#include "vector.h"
#include "file.h"

void* parse_thread_func(void *arg);
void parse_trade(const char *json_msg);

#endif // PARSER_H