/**
 * @file renderer.c
 * @brief Реализация системы рендеринга с двойной буферизацией
 */

#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Renderer* renderer_create(const char* title, int width, int height) {
    if (!title) {
        return NULL;
    }
    
    Renderer* renderer = (Renderer*)calloc(1, sizeof(Renderer));
    if (!renderer) {
        return NULL;
    }
    
    // Инициализируем SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Ошибка инициализации SDL: %s\n", SDL_GetError());
        free(renderer);
        return NULL;
    }
    
    // Создаем окно
    renderer->window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_SHOWN
    );
    
    if (!renderer->window) {
        printf("Ошибка создания окна: %s\n", SDL_GetError());
        free(renderer);
        return NULL;
    }
    
    // Создаем рендерер
    renderer->sdl_renderer = SDL_CreateRenderer(
        renderer->window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    
    if (!renderer->sdl_renderer) {
        printf("Ошибка создания рендерера: %s\n", SDL_GetError());
        SDL_DestroyWindow(renderer->window);
        free(renderer);
        return NULL;
    }
    
    // Создаем буферы для двойной буферизации
    renderer->front_buffer = SDL_CreateTexture(
        renderer->sdl_renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        width, height
    );
    
    renderer->back_buffer = SDL_CreateTexture(
        renderer->sdl_renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        width, height
    );
    
    if (!renderer->front_buffer || !renderer->back_buffer) {
        printf("Ошибка создания буферов: %s\n", SDL_GetError());
        renderer_destroy(renderer);
        return NULL;
    }
    
    // Инициализируем значения по умолчанию
    renderer->static_count = 0;
    renderer->dynamic_count = 0;
    renderer->camera = NULL;
    renderer->clear_color.r = 50;
    renderer->clear_color.g = 50;
    renderer->clear_color.b = 50;
    renderer->clear_color.a = 255;
    renderer->vsync_enabled = true;
    renderer->buffer_swap_pending = false;
    
    return renderer;
}

void renderer_destroy(Renderer* renderer) {
    if (!renderer) {
        return;
    }
    
    if (renderer->front_buffer) {
        SDL_DestroyTexture(renderer->front_buffer);
    }
    if (renderer->back_buffer) {
        SDL_DestroyTexture(renderer->back_buffer);
    }
    if (renderer->sdl_renderer) {
        SDL_DestroyRenderer(renderer->sdl_renderer);
    }
    if (renderer->window) {
        SDL_DestroyWindow(renderer->window);
    }
    
    SDL_Quit();
    free(renderer);
}

void renderer_set_camera(Renderer* renderer, Camera* camera) {
    if (renderer) {
        renderer->camera = camera;
    }
}

void renderer_set_clear_color(Renderer* renderer, Color color) {
    if (renderer) {
        renderer->clear_color = color;
    }
}

void renderer_set_vsync(Renderer* renderer, bool enabled) {
    if (renderer) {
        renderer->vsync_enabled = enabled;
        SDL_GL_SetSwapInterval(enabled ? 1 : 0);
    }
}

void renderer_add_sprite(Renderer* renderer, Sprite* sprite) {
    if (!renderer || !sprite) {
        return;
    }
    
    if (sprite->type == SPRITE_STATIC) {
        if (renderer->static_count < MAX_SPRITES) {
            renderer->static_sprites[renderer->static_count++] = sprite;
        }
    } else {
        if (renderer->dynamic_count < MAX_SPRITES) {
            renderer->dynamic_sprites[renderer->dynamic_count++] = sprite;
        }
    }
}

void renderer_remove_sprite(Renderer* renderer, Sprite* sprite) {
    if (!renderer || !sprite) {
        return;
    }
    
    if (sprite->type == SPRITE_STATIC) {
        for (int i = 0; i < renderer->static_count; i++) {
            if (renderer->static_sprites[i] == sprite) {
                // Сдвигаем массив
                for (int j = i; j < renderer->static_count - 1; j++) {
                    renderer->static_sprites[j] = renderer->static_sprites[j + 1];
                }
                renderer->static_count--;
                break;
            }
        }
    } else {
        for (int i = 0; i < renderer->dynamic_count; i++) {
            if (renderer->dynamic_sprites[i] == sprite) {
                // Сдвигаем массив
                for (int j = i; j < renderer->dynamic_count - 1; j++) {
                    renderer->dynamic_sprites[j] = renderer->dynamic_sprites[j + 1];
                }
                renderer->dynamic_count--;
                break;
            }
        }
    }
}

void renderer_clear_sprites(Renderer* renderer) {
    if (renderer) {
        renderer->static_count = 0;
        renderer->dynamic_count = 0;
    }
}

void renderer_begin_frame(Renderer* renderer) {
    if (!renderer) {
        return;
    }
    
    // Очищаем задний буфер
    SDL_SetRenderTarget(renderer->sdl_renderer, renderer->back_buffer);
    SDL_SetRenderDrawColor(
        renderer->sdl_renderer,
        renderer->clear_color.r,
        renderer->clear_color.g,
        renderer->clear_color.b,
        renderer->clear_color.a
    );
    SDL_RenderClear(renderer->sdl_renderer);
}

void renderer_render_sprites(Renderer* renderer) {
    if (!renderer || !renderer->camera) {
        return;
    }
    
    // Рендерим статические спрайты
    for (int i = 0; i < renderer->static_count; i++) {
        Sprite* sprite = renderer->static_sprites[i];
        if (!sprite || !sprite->visible) {
            continue;
        }
        
        // Проверяем видимость
        Rect sprite_rect = {
            sprite->position.x,
            sprite->position.y,
            sprite->size.x,
            sprite->size.y
        };
        
        if (!camera_is_visible(renderer->camera, &sprite_rect)) {
            continue;
        }
        
        // Преобразуем координаты в экранные
        Vector2 screen_pos = camera_world_to_screen(renderer->camera, sprite->position);
        
        // Выбираем текстуру в зависимости от состояния освещения
        SDL_Texture* texture = sprite->texture;
        if (sprite->light_state == LIGHT_SHADOW && sprite->shadow_texture) {
            texture = sprite->shadow_texture;
        }
        
        // Рендерим спрайт
        SDL_Rect dest_rect = {
            (int)screen_pos.x,
            (int)screen_pos.y,
            (int)sprite->size.x,
            (int)sprite->size.y
        };
        
        SDL_SetTextureColorMod(texture, sprite->tint.r, sprite->tint.g, sprite->tint.b);
        SDL_SetTextureAlphaMod(texture, sprite->tint.a);
        SDL_RenderCopy(renderer->sdl_renderer, texture, NULL, &dest_rect);
    }
    
    // Рендерим динамические спрайты
    for (int i = 0; i < renderer->dynamic_count; i++) {
        Sprite* sprite = renderer->dynamic_sprites[i];
        if (!sprite || !sprite->visible) {
            continue;
        }
        
        // Проверяем видимость
        Rect sprite_rect = {
            sprite->position.x,
            sprite->position.y,
            sprite->size.x,
            sprite->size.y
        };
        
        if (!camera_is_visible(renderer->camera, &sprite_rect)) {
            continue;
        }
        
        // Преобразуем координаты в экранные
        Vector2 screen_pos = camera_world_to_screen(renderer->camera, sprite->position);
        
        // Выбираем текстуру в зависимости от состояния освещения
        SDL_Texture* texture = sprite->texture;
        if (sprite->light_state == LIGHT_SHADOW && sprite->shadow_texture) {
            texture = sprite->shadow_texture;
        }
        
        // Рендерим спрайт
        SDL_Rect dest_rect = {
            (int)screen_pos.x,
            (int)screen_pos.y,
            (int)sprite->size.x,
            (int)sprite->size.y
        };
        
        SDL_SetTextureColorMod(texture, sprite->tint.r, sprite->tint.g, sprite->tint.b);
        SDL_SetTextureAlphaMod(texture, sprite->tint.a);
        SDL_RenderCopy(renderer->sdl_renderer, texture, NULL, &dest_rect);
    }
}

void renderer_end_frame(Renderer* renderer) {
    if (!renderer) {
        return;
    }
    
    // Возвращаемся к обычному рендерингу
    SDL_SetRenderTarget(renderer->sdl_renderer, NULL);
    
    // Атомарно меняем буферы
    SDL_Texture* temp = renderer->front_buffer;
    renderer->front_buffer = renderer->back_buffer;
    renderer->back_buffer = temp;
    
    // Рендерим готовый кадр на экран
    SDL_RenderCopy(renderer->sdl_renderer, renderer->front_buffer, NULL, NULL);
    SDL_RenderPresent(renderer->sdl_renderer);
}

SDL_Renderer* renderer_get_sdl_renderer(Renderer* renderer) {
    return renderer ? renderer->sdl_renderer : NULL;
}

bool renderer_is_vsync_enabled(const Renderer* renderer) {
    return renderer ? renderer->vsync_enabled : false;
}
