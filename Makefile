# Compiler and flags
CC      = gcc
CFLAGS  = -Wall
LDFLAGS = -ljansson -lwebsockets -lpthread -lm

# Cross-compiler setup for Raspberry Pi
PI_CC      = arm-linux-gnueabihf-gcc
PI_SYSROOT = $(HOME)/rpi-sysroot
PI_CFLAGS  = --sysroot=$(PI_SYSROOT) -I$(PI_SYSROOT)/usr/include -I$(PI_SYSROOT)/usr/include/arm-linux-gnueabihf -Wall
PI_LDFLAGS = \
  -L/home/maria/rpi-sysroot/usr/lib \
  -L/home/maria/rpi-sysroot/usr/lib/arm-linux-gnueabihf \
  -L/home/maria/rpi-sysroot/lib/arm-linux-gnueabihf \
  -Wl,-rpath-link=/home/maria/rpi-sysroot/usr/lib/arm-linux-gnueabihf \
  -Wl,-rpath-link=/home/maria/rpi-sysroot/lib/arm-linux-gnueabihf \
  -ljansson -lwebsockets -lm -lssl -lcrypto -ldl -lz -lcap -lpthread -latomic


# Directories
SRC_DIR = src src/metrics src/structures src/websocket
INCLUDE_DIR = include/metrics include/structures include/websocket
OBJ_DIR = obj
PI_OBJ_DIR = obj-pi

# Source files
SRCS = $(foreach dir,$(SRC_DIR),$(wildcard $(dir)/*.c))
# Native object files
OBJS = $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(SRCS))
# Raspberry Pi object files
PI_OBJS = $(patsubst src/%.c,$(PI_OBJ_DIR)/%.o,$(SRCS))

# Output binaries
TARGET = crypto
PI_TARGET = crypto-pi

# Include paths
INCLUDES = $(foreach dir,$(INCLUDE_DIR),-I$(dir))

# Rules
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(OBJ_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

pi: $(PI_TARGET)

$(PI_TARGET): $(PI_OBJS)
	$(PI_CC) $(PI_CFLAGS) $(PI_OBJS) -o $(PI_TARGET) $(PI_LDFLAGS)

$(PI_OBJ_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(PI_CC) $(PI_CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(PI_OBJ_DIR)
	rm -f $(TARGET) $(PI_TARGET)

.PHONY: all clean