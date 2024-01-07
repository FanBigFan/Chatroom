CC = gcc
CFLAGS = -g -Wall -pthread
BUILD_DIR = build
SRC_DIR = src

SRCS := $(wildcard $(SRC_DIR)/*.c)

OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%, $(SRCS))

all: create_build_dir $(OBJS)

$(BUILD_DIR)/%: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

create_build_dir:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all create_build_dir clean
