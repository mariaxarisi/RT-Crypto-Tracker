#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void create_files();
void write_trade(const char *symbol, const char *timestamp, const char *price, const char *size);

#endif // FILE_H