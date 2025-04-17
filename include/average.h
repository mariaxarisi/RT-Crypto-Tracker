#ifndef AVERAGE_H
#define AVERAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

long long current_timestamp_ms();
void calculate_moving_average(int i, long long now);
void* average_thread_func(void *arg);

#endif