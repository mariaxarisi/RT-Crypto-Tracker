#ifndef PARSER_H
#define PARSER_H

#include <jansson.h>

void* parse_thread_func(void *arg);
void parse_trade(const char *json_msg);

#endif // PARSER_H