# Define variables
CC = gcc
CFLAGS = -Wall -Wextra -Isrc/lib -lcurl -lcjson
SRC_DIR = src
BUILD_DIR = build

# Define sources and objects for the library
LIB_SRCS := $(wildcard $(SRC_DIR)/lib/*.c)
LIB_OBJS := $(LIB_SRCS:$(SRC_DIR)/lib/%.c=$(BUILD_DIR)/%.o)

# Automatically include all header files in the lib directory
LIB_HEADERS := $(wildcard $(SRC_DIR)/lib/*.h)

# Define sources and objects for the cmd target
CMD_SRCS := $(wildcard $(SRC_DIR)/cmd/*.c)
CMD_OBJS := $(CMD_SRCS:$(SRC_DIR)/cmd/%.c=$(BUILD_DIR)/%.o)

# Define targets
all: cmd

cmd: $(CMD_OBJS) $(LIB_OBJS)
		$(CC) -o $(BUILD_DIR)/mc_launcher $^ $(CFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/cmd/%.c | $(BUILD_DIR)
		$(CC) -c -o $@ $< $(CFLAGS) 

$(BUILD_DIR)/%.o: $(SRC_DIR)/lib/%.c | $(BUILD_DIR)
		$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR):
		mkdir -p $@

lib: $(LIB_OBJS)

$(LIB_OBJS): | $(BUILD_DIR)

clean:
		rm -rf $(BUILD_DIR)

.PHONY: all cmd lib clean
