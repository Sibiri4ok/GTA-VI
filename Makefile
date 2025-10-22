# Makefile для 2D игрового движка
# Компилятор и флаги
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2 -g
LDFLAGS = -lSDL2 -lSDL2_image -lm

# Директории
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
ASSETS_DIR = assets

# Исходные файлы
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Целевой исполняемый файл
TARGET = $(BUILD_DIR)/game_engine

# Создание демо-ресурсов
create-assets:
	@echo "Создание демо-ресурсов..."
	@mkdir -p $(ASSETS_DIR)
	@echo "Создание простой карты..."
	@python3 -c "from PIL import Image; img = Image.new('RGBA', (50, 50), (255, 255, 255, 255)); pixels = img.load(); [pixels.__setitem__((x, y), (139, 69, 19, 255) if x == 0 or x == 49 or y == 0 or y == 49 else (50, 50, 50, 255) if (x + y) % 8 == 0 else (100, 200, 100, 255) if (x + y) % 12 == 0 else (255, 255, 255, 255)) for y in range(50) for x in range(50)]; img.save('$(ASSETS_DIR)/map.png')"
	@echo "Создание спрайта игрока..."
	@python3 -c "from PIL import Image; img = Image.new('RGBA', (32, 32), (255, 0, 0, 255)); pixels = img.load(); [pixels.__setitem__((x, y), (255, 255, 255, 255) if (x == 8 and y == 8) or (x == 24 and y == 8) else (0, 0, 0, 255) if (x == 8 and y == 10) or (x == 24 and y == 10) else (255, 0, 0, 255)) for y in range(32) for x in range(32)]; img.save('$(ASSETS_DIR)/player.png')"
	@echo "Ресурсы созданы в $(ASSETS_DIR)/"

# Основная цель
all: $(TARGET)

# Создание директорий
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Компиляция объектных файлов
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Сборка исполняемого файла
$(TARGET): $(OBJECTS) | $(BUILD_DIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# Очистка
clean:
	rm -rf $(BUILD_DIR)

# Запуск
run: $(TARGET)
	./$(TARGET)

# Установка зависимостей (Ubuntu/Debian)
install-deps:
	sudo apt-get update
	sudo apt-get install -y libsdl2-dev libsdl2-image-dev

# Создание директории для ресурсов
assets:
	mkdir -p $(ASSETS_DIR)

.PHONY: all clean run install-deps assets
