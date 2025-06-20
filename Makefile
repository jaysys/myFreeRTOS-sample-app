# Makefile for myTaskApp on macOS using FreeRTOS POSIX Simulator

# Target executable
TARGET = myTaskApp

# Build directory
BUILD_DIR = build

# Compiler and linker
CC = gcc
LDFLAGS = -pthread

# Directories
FREERTOS_DIR = src/FreeRTOS-Kernel-src
APP_SRC_DIR = src

# Source directories
VPATH = $(APP_SRC_DIR) \
        $(FREERTOS_DIR) \
        $(FREERTOS_DIR)/portable/ThirdParty/GCC/Posix \
        $(FREERTOS_DIR)/portable/MemMang

# Include paths
INCLUDES = -I$(APP_SRC_DIR) \
           -I$(FREERTOS_DIR)/include \
           -I$(FREERTOS_DIR)/portable/ThirdParty/GCC/Posix

# Compiler flags
CFLAGS = $(INCLUDES) -g -Wall -Wextra -DprojCOVERAGE_TEST=0

# Source files
C_SRCS = main.c \
         tasks.c \
         queue.c \
         list.c \
         timers.c \
         event_groups.c \
         stream_buffer.c \
         heap_4.c \
         port.c \
         utils/wait_for_event.c

# Object files
OBJS = $(addprefix $(BUILD_DIR)/, $(C_SRCS:.c=.o))

# Default target
all: $(BUILD_DIR)/$(TARGET)

# Linking rule
$(BUILD_DIR)/$(TARGET): $(OBJS)
	@echo "Linking..."
	$(CC) $(OBJS) $(LDFLAGS) -o $@
	@echo "Build complete: $(@F)"

# Compilation rule for .c files
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
clean:
	@echo "Cleaning..."
	@rm -rf $(BUILD_DIR)

.PHONY: all clean
