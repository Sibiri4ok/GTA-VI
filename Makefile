CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O3 -MMD -MP
LDFLAGS = -lSDL2 -lm

SRC_DIR = src
DEMO_DIR = demo
INCLUDE_DIR = include
BUILD_DIR = build
TEST_DIR = tests
DEPS_DIR = deps

# Library sources (src directory)
LIB_SOURCES = $(shell find $(SRC_DIR) -type f -name '*.c')
LIB_OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/lib/%.o,$(LIB_SOURCES))

# Game sources (game directory with main.c)
DEMO_SOURCES = $(shell find $(DEMO_DIR) -type f -name '*.c')
DEMO_OBJECTS = $(patsubst $(DEMO_DIR)/%.c,$(BUILD_DIR)/$(DEMO_DIR)/%.o,$(DEMO_SOURCES))

DEPS_OBJECTS = $(patsubst $(DEPS_DIR)/%.c,$(BUILD_DIR)/deps/%.o,$(wildcard $(DEPS_DIR)/*.c))
DEPS = $(LIB_OBJECTS:.o=.d) $(DEMO_OBJECTS:.o=.d)

TARGET = $(BUILD_DIR)/demo_game
TEST_SOURCES = $(shell find $(TEST_DIR) -type f -name '*.c' ! -name 'test_framework.c')
TEST_OBJECTS = $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/tests/%.o,$(TEST_SOURCES))
TEST_FRAMEWORK_OBJ = $(BUILD_DIR)/tests/test_framework.o
TEST_TARGET = $(BUILD_DIR)/test_runner
SRC_OBJECTS_FOR_TESTS = $(LIB_OBJECTS)

.PHONY: all clean run test fmt check-fmt

all: $(TARGET)

$(BUILD_DIR) $(BUILD_DIR)/tests:
	@mkdir -p $@

# Compile library sources (src/)
$(BUILD_DIR)/lib/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -I$(SRC_DIR) -I$(DEPS_DIR) -c $< -o $@

# Compile demo sources (demo/)
$(BUILD_DIR)/demo/%.o: $(DEMO_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -I$(SRC_DIR) -I$(DEPS_DIR) -c $< -o $@

$(BUILD_DIR)/deps/%.o: $(DEPS_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) -std=c99 -O2 -w -I$(DEPS_DIR) -c $< -o $@

$(TARGET): $(LIB_OBJECTS) $(DEMO_OBJECTS) $(DEPS_OBJECTS) | $(BUILD_DIR)
	$(CC) $(LIB_OBJECTS) $(DEMO_OBJECTS) $(DEPS_OBJECTS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/tests/test_framework.o: $(TEST_DIR)/test_framework.c | $(BUILD_DIR)/tests
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -I$(SRC_DIR) -I$(DEPS_DIR) -c $< -o $@

$(BUILD_DIR)/tests/%.o: $(TEST_DIR)/%.c | $(BUILD_DIR)/tests
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -I$(SRC_DIR) -I$(DEPS_DIR) -I$(TEST_DIR) -c $< -o $@

$(TEST_TARGET): $(TEST_FRAMEWORK_OBJ) $(TEST_OBJECTS) $(SRC_OBJECTS_FOR_TESTS) $(DEPS_OBJECTS) | $(BUILD_DIR)
	$(CC) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGET)
	@./$(TEST_TARGET)

# --- clang format targets ---
FMT_SOURCES := $(shell find $(SRC_DIR) $(DEMO_DIR) $(INCLUDE_DIR) -path "$(INCLUDE_DIR)/external" -prune -o -name '*.c' -o -name '*.h' -print)

fmt:
	clang-format -i $(FMT_SOURCES)
check-fmt:
	@clang-format --dry-run --Werror $(FMT_SOURCES)

-include $(DEPS)
