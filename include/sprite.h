/**
 * @file sprite.h
 * @brief Система спрайтов и анимации
 * 
 * Модуль отвечает за управление спрайтами, их позиционированием,
 * анимацией и рендерингом. Поддерживает статические и динамические
 * спрайты с возможностью изменения состояния (например, освещение/тень).
 */

#ifndef SPRITE_H
#define SPRITE_H

#include "engine.h"

// Типы спрайтов
typedef enum {
    SPRITE_STATIC,      // Статический спрайт (фон, здания)
    SPRITE_DYNAMIC,     // Динамический спрайт (персонажи)
    SPRITE_SHADOW       // Спрайт тени
} SpriteType;

// Состояния освещения
typedef enum {
    LIGHT_BRIGHT,       // Освещенный
    LIGHT_SHADOW        // В тени
} LightState;

// Структура спрайта
typedef struct Sprite {
    SDL_Texture* texture;
    SDL_Texture* shadow_texture;  // Альтернативная текстура для тени
    Vector2 position;
    Vector2 size;
    float rotation;
    Color tint;
    SpriteType type;
    LightState light_state;
    bool visible;
    int layer;
    void* user_data;     // Пользовательские данные
} Sprite;

// Функции работы со спрайтами
Sprite* sprite_create(SDL_Renderer* renderer, const char* image_path);
Sprite* sprite_create_with_shadow(SDL_Renderer* renderer, 
                                  const char* image_path, 
                                  const char* shadow_path);
void sprite_destroy(Sprite* sprite);

void sprite_set_position(Sprite* sprite, float x, float y);
void sprite_set_size(Sprite* sprite, float w, float h);
void sprite_set_light_state(Sprite* sprite, LightState state);
void sprite_set_layer(Sprite* sprite, int layer);
void sprite_set_visible(Sprite* sprite, bool visible);

Vector2 sprite_get_position(const Sprite* sprite);
Vector2 sprite_get_size(const Sprite* sprite);
bool sprite_is_visible(const Sprite* sprite);

#endif // SPRITE_H
