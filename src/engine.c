/**
 * @file engine.c
 * @brief Основная реализация игрового движка
 */

#include "engine.h"
#include "app.h"
#include <stdio.h>
#include <stdlib.h>

// Глобальные переменные движка
static App* g_app = NULL;
static bool g_initialized = false;

bool engine_init(void) {
    if (g_initialized) {
        return true;
    }
    
    // Инициализируем SDL_image
    int img_flags = IMG_INIT_PNG | IMG_INIT_JPG;
    if (!(IMG_Init(img_flags) & img_flags)) {
        printf("Ошибка инициализации SDL_image: %s\n", IMG_GetError());
        return false;
    }
    
    g_initialized = true;
    return true;
}

void engine_cleanup(void) {
    if (g_app) {
        app_destroy(g_app);
        g_app = NULL;
    }
    
    if (g_initialized) {
        IMG_Quit();
        g_initialized = false;
    }
}

void engine_update(float delta_time) {
    if (g_app) {
        app_update(g_app);
    }
}

void engine_render(void) {
    if (g_app) {
        app_render(g_app);
    }
}

bool engine_is_running(void) {
    return g_app ? app_is_running(g_app) : false;
}

// Функции для создания и управления приложением
App* engine_create_app(const char* title, int width, int height) {
    if (!g_initialized) {
        if (!engine_init()) {
            return NULL;
        }
    }
    
    g_app = app_create(title, width, height);
    if (g_app) {
        if (!app_init(g_app)) {
            app_destroy(g_app);
            g_app = NULL;
        }
    }
    
    return g_app;
}

void engine_run_app(void) {
    if (g_app) {
        app_run(g_app);
    }
}

void engine_quit_app(void) {
    if (g_app) {
        app_quit(g_app);
    }
}

float engine_get_fps(void) {
    return g_app ? app_get_fps(g_app) : 0.0f;
}

float engine_get_delta_time(void) {
    return g_app ? app_get_delta_time(g_app) : 0.0f;
}
