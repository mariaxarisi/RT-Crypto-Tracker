#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

void ensure_directory_exists(const char *path);
void clear_directory(const char *path);
void create_empty_file(const char *path);
void create_files();
void write_trade(const char *symbol, const char *timestamp, const char *price, const char *size);
void write_average(const char *symbol, long long timestamp, double average);
void write_pearson(const char *symbol, long long timestamp, const char *related_symbol, double pearson);
void write_delay(long long timestamp, long long delay, char type);
void write_cpu_usage(long long timestamp, double cpu_usage);

#endif // FILE_H