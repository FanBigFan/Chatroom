CC = gcc
CFLAGS = -g -Wall -pthread
BUILD_DIR = build
SRC_DIR = src

# 获取所有源文件列表
SRCS := $(wildcard $(SRC_DIR)/*.c)
# 生成所有目标文件列表
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%, $(SRCS))

# 默认目标，构建所有目标文件
all: create_build_dir $(OBJS)

# 生成目标文件规则
$(BUILD_DIR)/%: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

# 创建构建目录
create_build_dir:
	mkdir -p $(BUILD_DIR)

# 清理目标文件和构建目录
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all create_build_dir clean
