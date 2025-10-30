CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2 -MMD -MP
LDFLAGS = -lSDL2 -lSDL2_image -lm

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
ASSETS_DIR = assets

SOURCES = $(shell find $(SRC_DIR) -type f -name '*.c')
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))
DEPS = $(OBJECTS:.o=.d)

TARGET = $(BUILD_DIR)/game_engine

.PHONY: all clean run install-deps

all: $(TARGET)

$(BUILD_DIR):
	@mkdir -p $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(TARGET): $(OBJECTS) | $(BUILD_DIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

install-deps:
	sudo apt-get update
	sudo apt-get install -y libsdl2-dev libsdl2-image-dev

# --- clang format targets ---
FMT_SOURCES := $(shell find $(SRC_DIR) $(INCLUDE_DIR) -name '*.c' -o -name '*.h')

fmt:
	clang-format -i $(FMT_SOURCES)

check-fmt:
	@clang-format --dry-run --Werror $(FMT_SOURCES)

-include $(DEPS)
