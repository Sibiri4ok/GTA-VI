CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2 -MMD -MP
LDFLAGS = -lSDL2 -lm

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
TEST_DIR = tests
DEPS_DIR = deps

SOURCES = $(shell find $(SRC_DIR) -type f -name '*.c')
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))
DEPS_OBJECTS = $(patsubst $(DEPS_DIR)/%.c,$(BUILD_DIR)/deps/%.o,$(wildcard $(DEPS_DIR)/*.c))
DEPS = $(OBJECTS:.o=.d)

TARGET = $(BUILD_DIR)/game_engine
TEST_SOURCES = $(shell find $(TEST_DIR) -type f -name '*.c' ! -name 'test_framework.c')
TEST_OBJECTS = $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/tests/%.o,$(TEST_SOURCES))
TEST_FRAMEWORK_OBJ = $(BUILD_DIR)/tests/test_framework.o
TEST_TARGET = $(BUILD_DIR)/test_runner
SRC_OBJECTS_FOR_TESTS = $(filter-out $(BUILD_DIR)/app/main.o,$(OBJECTS))

.PHONY: all clean run test fmt check-fmt

all: $(TARGET)

$(BUILD_DIR) $(BUILD_DIR)/tests:
	@mkdir -p $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -I$(SRC_DIR) -I$(DEPS_DIR) -c $< -o $@

$(BUILD_DIR)/deps/%.o: $(DEPS_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) -std=c99 -O2 -w -I$(DEPS_DIR) -c $< -o $@

$(TARGET): $(OBJECTS) $(DEPS_OBJECTS) | $(BUILD_DIR)
	$(CC) $(OBJECTS) $(DEPS_OBJECTS) -o $@ $(LDFLAGS)

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
FMT_SOURCES := $(shell find $(SRC_DIR) $(INCLUDE_DIR) -path "$(INCLUDE_DIR)/external" -prune -o -name '*.c' -o -name '*.h' -print)

fmt:
	clang-format -i $(FMT_SOURCES)
check-fmt:
	@clang-format --dry-run --Werror $(FMT_SOURCES)

-include $(DEPS)
