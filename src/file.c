#include "../include/file.h"
#include <sys/stat.h>
#include <sys/types.h>

void create_files() {
    // Make sure the logs/trades directory exists
    struct stat st = {0};
    if (stat("./logs/trades", &st) == -1) {
        if (mkdir("./logs", 0755) == -1) {
            perror("Error creating ../logs");
        }
        if (mkdir("./logs/trades", 0755) == -1) {
            perror("Error creating ./logs/trades");
        }
    }
    
    const char *symbols[] = {
        "BTC-USDT", "ADA-USDT", "ETH-USDT", "DOGE-USDT",
        "XRP-USDT", "SOL-USDT", "LTC-USDT", "BNB-USDT"
    };

    if(system("rm -f ./logs/trades/*.csv") != 0){
        perror("Error deleting old files");
    }

    for (size_t i = 0; i < sizeof(symbols) / sizeof(symbols[0]); i++) {
        char filename[64];
        snprintf(filename, sizeof(filename), "./logs/trades/%s.csv", symbols[i]);

        FILE *file = fopen(filename, "w");
        if (file) {
            fclose(file);
        } else {
            perror("Error creating file");
        }
    }
}

void write_trade(const char *symbol, const char *timestamp, const char *price, const char *size) {
    char filename[64];
    snprintf(filename, sizeof(filename), "./logs/trades/%s.csv", symbol);

    FILE *file = fopen(filename, "a");
    if (file) {
        fprintf(file, "%s, %s, %s\n", timestamp, price, size);
        fclose(file);
    }
}