/**
 * @file sprite.c
 * @brief Реализация системы спрайтов и анимации
 */

#include "sprite.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Sprite* sprite_create(SDL_Renderer* renderer, const char* image_path) {
    if (!renderer || !image_path) {
        return NULL;
    }
    
    Sprite* sprite = (Sprite*)calloc(1, sizeof(Sprite));
    if (!sprite) {
        return NULL;
    }
    
    // Загружаем текстуру
    SDL_Surface* surface = IMG_Load(image_path);
    if (!surface) {
        printf("Ошибка загрузки изображения %s: %s\n", image_path, IMG_GetError());
        free(sprite);
        return NULL;
    }
    
    sprite->texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    if (!sprite->texture) {
        printf("Ошибка создания текстуры: %s\n", SDL_GetError());
        free(sprite);
        return NULL;
    }
    
    // Получаем размеры текстуры
    int w, h;
    SDL_QueryTexture(sprite->texture, NULL, NULL, &w, &h);
    sprite->size.x = (float)w;
    sprite->size.y = (float)h;
    
    // Устанавливаем значения по умолчанию
    sprite->position.x = 0.0f;
    sprite->position.y = 0.0f;
    sprite->rotation = 0.0f;
    sprite->tint.r = 255;
    sprite->tint.g = 255;
    sprite->tint.b = 255;
    sprite->tint.a = 255;
    sprite->type = SPRITE_DYNAMIC;
    sprite->light_state = LIGHT_BRIGHT;
    sprite->visible = true;
    sprite->layer = 0;
    sprite->user_data = NULL;
    
    return sprite;
}

Sprite* sprite_create_with_shadow(SDL_Renderer* renderer, 
                                  const char* image_path, 
                                  const char* shadow_path) {
    Sprite* sprite = sprite_create(renderer, image_path);
    if (!sprite) {
        return NULL;
    }
    
    // Загружаем текстуру тени
    if (shadow_path) {
        SDL_Surface* shadow_surface = IMG_Load(shadow_path);
        if (shadow_surface) {
            sprite->shadow_texture = SDL_CreateTextureFromSurface(renderer, shadow_surface);
            SDL_FreeSurface(shadow_surface);
        }
    }
    
    return sprite;
}

void sprite_destroy(Sprite* sprite) {
    if (!sprite) {
        return;
    }
    
    if (sprite->texture) {
        SDL_DestroyTexture(sprite->texture);
    }
    if (sprite->shadow_texture) {
        SDL_DestroyTexture(sprite->shadow_texture);
    }
    
    free(sprite);
}

void sprite_set_position(Sprite* sprite, float x, float y) {
    if (sprite) {
        sprite->position.x = x;
        sprite->position.y = y;
    }
}

void sprite_set_size(Sprite* sprite, float w, float h) {
    if (sprite) {
        sprite->size.x = w;
        sprite->size.y = h;
    }
}

void sprite_set_light_state(Sprite* sprite, LightState state) {
    if (sprite) {
        sprite->light_state = state;
    }
}

void sprite_set_layer(Sprite* sprite, int layer) {
    if (sprite) {
        sprite->layer = layer;
    }
}

void sprite_set_visible(Sprite* sprite, bool visible) {
    if (sprite) {
        sprite->visible = visible;
    }
}

Vector2 sprite_get_position(const Sprite* sprite) {
    Vector2 pos = {0, 0};
    if (sprite) {
        pos = sprite->position;
    }
    return pos;
}

Vector2 sprite_get_size(const Sprite* sprite) {
    Vector2 size = {0, 0};
    if (sprite) {
        size = sprite->size;
    }
    return size;
}

bool sprite_is_visible(const Sprite* sprite) {
    return sprite ? sprite->visible : false;
}
