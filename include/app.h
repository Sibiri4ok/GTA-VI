/**
 * @file app.h
 * @brief Главное приложение и игровой цикл
 * 
 * Модуль управляет основным игровым циклом, обработкой событий
 * и координацией работы всех систем движка. Реализует принцип
 * разделения логики (update) и рендеринга (render).
 */

#ifndef APP_H
#define APP_H

#include "engine.h"
#include "renderer.h"
#include "camera.h"
#include "sprite.h"
#include "map.h"

// Состояние приложения
typedef enum {
    APP_RUNNING,
    APP_PAUSED,
    APP_QUITTING
} AppState;

// Структура приложения
typedef struct App {
    AppState state;
    bool running;
    uint32_t last_frame_time;
    float delta_time;
    float fps;
    
    // Системы движка
    Renderer* renderer;
    Camera* camera;
    Map* map;
    
    // Игровые объекты
    Sprite* player;
    Vector2 player_position;
    Vector2 player_velocity;
} App;

// Функции приложения
App* app_create(const char* title, int width, int height);
void app_destroy(App* app);

bool app_init(App* app);
void app_run(App* app);
void app_quit(App* app);

// Игровой цикл
void app_update(App* app);
void app_render(App* app);
void app_handle_events(App* app);

// Утилиты
float app_get_delta_time(const App* app);
float app_get_fps(const App* app);
bool app_is_running(const App* app);

#endif // APP_H
