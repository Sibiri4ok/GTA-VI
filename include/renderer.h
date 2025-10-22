/**
 * @file renderer.h
 * @brief Система рендеринга с двойной буферизацией
 * 
 * Модуль отвечает за рендеринг сцены с использованием двойной буферизации.
 * Рендеринг происходит в невидимый буфер, который затем атомарно
 * заменяется с видимым буфером для обеспечения плавности.
 */

#ifndef RENDERER_H
#define RENDERER_H

#include "engine.h"
#include "sprite.h"
#include "camera.h"

// Структура рендерера
typedef struct Renderer {
    SDL_Window* window;
    SDL_Renderer* sdl_renderer;
    
    // Двойная буферизация
    SDL_Texture* front_buffer;
    SDL_Texture* back_buffer;
    bool buffer_swap_pending;
    
    // Списки спрайтов для рендеринга
    Sprite* static_sprites[MAX_SPRITES];
    Sprite* dynamic_sprites[MAX_SPRITES];
    int static_count;
    int dynamic_count;
    
    // Камера
    Camera* camera;
    
    // Настройки рендеринга
    Color clear_color;
    bool vsync_enabled;
} Renderer;

// Функции рендерера
Renderer* renderer_create(const char* title, int width, int height);
void renderer_destroy(Renderer* renderer);

void renderer_set_camera(Renderer* renderer, Camera* camera);
void renderer_set_clear_color(Renderer* renderer, Color color);
void renderer_set_vsync(Renderer* renderer, bool enabled);

// Управление спрайтами
void renderer_add_sprite(Renderer* renderer, Sprite* sprite);
void renderer_remove_sprite(Renderer* renderer, Sprite* sprite);
void renderer_clear_sprites(Renderer* renderer);

// Рендеринг
void renderer_begin_frame(Renderer* renderer);
void renderer_render_sprites(Renderer* renderer);
void renderer_end_frame(Renderer* renderer);

// Утилиты
SDL_Renderer* renderer_get_sdl_renderer(Renderer* renderer);
bool renderer_is_vsync_enabled(const Renderer* renderer);

#endif // RENDERER_H
