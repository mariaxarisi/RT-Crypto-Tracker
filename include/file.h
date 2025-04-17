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

#endif // FILE_H