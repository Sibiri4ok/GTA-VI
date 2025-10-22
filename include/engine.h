/**
 * @file engine.h
 * @brief Основной заголовочный файл игрового движка
 * 
 * Этот файл содержит основные структуры данных и функции для работы
 * с 2D игровым движком. Движок построен на принципах изотермичности
 * и разделения логики от рендеринга.
 */

#ifndef ENGINE_H
#define ENGINE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdint.h>

// Константы движка
#define ENGINE_VERSION "1.0.0"
#define TARGET_FPS 60
#define CAMERA_WIDTH 400
#define CAMERA_HEIGHT 300
#define MAX_SPRITES 1000
#define MAX_LAYERS 10

// Типы данных
typedef struct Vector2 {
    float x, y;
} Vector2;

typedef struct Rect {
    float x, y, w, h;
} Rect;

typedef struct Color {
    uint8_t r, g, b, a;
} Color;

// Структуры движка
typedef struct Sprite Sprite;
typedef struct Camera Camera;
typedef struct Renderer Renderer;
typedef struct App App;

// Основные функции движка
bool engine_init(void);
void engine_cleanup(void);
void engine_update(float delta_time);
void engine_render(void);
bool engine_is_running(void);

// Функции для создания и управления приложением
App* engine_create_app(const char* title, int width, int height);
void engine_run_app(void);
void engine_quit_app(void);
float engine_get_fps(void);
float engine_get_delta_time(void);

#endif // ENGINE_H
