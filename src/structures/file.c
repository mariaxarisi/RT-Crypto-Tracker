#include "../../include/structures/file.h"

extern const int SYMBOL_COUNT;
extern const char *symbols[];

void ensure_directory_exists(const char *path) {
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        if (mkdir(path, 0755) == -1) {
            perror("Error creating directory");
        }
    }
}

void clear_directory(const char *path) {
    char command[128];
    snprintf(command, sizeof(command), "rm -f %s/*.csv", path);
    if (system(command) != 0) {
        perror("Error deleting old files");
    }
}

void create_empty_file(const char *path) {
    FILE *file = fopen(path, "w");
    if (file) {
        fclose(file);
    } else {
        perror("Error creating file");
    }
}

void create_files() {
    const char *directories[] = {"./logs", "./logs/trades", "./logs/average", "./logs/pearson"};

    // Ensure directories exist
    for (size_t i = 0; i < sizeof(directories) / sizeof(directories[0]); i++) {
        ensure_directory_exists(directories[i]);
    }

    // Clear old files
    for (size_t i = 1; i < sizeof(directories) / sizeof(directories[0]); i++) {
        clear_directory(directories[i]);
    }

    // Create new files
    for (size_t i = 0; i < SYMBOL_COUNT; i++) {
        char path[64];
        snprintf(path, sizeof(path), "./logs/trades/%s.csv", symbols[i]);
        create_empty_file(path);

        snprintf(path, sizeof(path), "./logs/average/%s.csv", symbols[i]);
        create_empty_file(path);

        snprintf(path, sizeof(path), "./logs/pearson/%s.csv", symbols[i]);
        create_empty_file(path);
    }
}

void write_trade(const char *symbol, const char *timestamp, const char *price, const char *size) {
    char filename[64];
    snprintf(filename, sizeof(filename), "./logs/trades/%s.csv", symbol);

    FILE *file = fopen(filename, "a");
    if (file) {
        fprintf(file, "%s, %s, %s\n", timestamp, price, size);
        fclose(file);
    }else {
        fprintf(stderr, "Failed to open file: %s\n", filename);
    }
}

void write_average(const char *symbol, long long timestamp, double average) {
    char filename[64];
    snprintf(filename, sizeof(filename), "./logs/average/%s.csv", symbol);

    FILE *file = fopen(filename, "a");
    if (file) {
        fprintf(file, "%lld, %.8f\n", timestamp, average);
        fclose(file);
    } else {
        fprintf(stderr, "Failed to open file: %s\n", filename);
    }
}

void write_pearson(const char *symbol, long long timestamp, const char *related_symbol, double pearson){
    char filename[64];
    snprintf(filename, sizeof(filename), "./logs/pearson/%s.csv", symbol);

    FILE *file = fopen(filename, "a");
    if (file) {
        fprintf(file, "%lld, %s, %.8f\n", timestamp, related_symbol, pearson);
        fclose(file);
    } else {
        fprintf(stderr, "Failed to open file: %s\n", filename);
    }
}