# Compiler and flags
CC = gcc
CFLAGS = -Wall -pthread -ljansson -lwebsockets -lm

# Directories
SRC_DIR = src src/metrics src/structures src/websocket
INCLUDE_DIR = include/metrics include/structures include/websocket
OBJ_DIR = obj
BIN_DIR = bin

# Source files
SRCS = $(foreach dir,$(SRC_DIR),$(wildcard $(dir)/*.c))
# Object files
OBJS = $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Output binary
TARGET = $(BIN_DIR)/crypto

# Include paths
INCLUDES = $(foreach dir,$(INCLUDE_DIR),-I$(dir))

# Rules
all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

$(OBJ_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean