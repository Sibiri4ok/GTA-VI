/**
 * @file map.h
 * @brief Система загрузки и управления картами
 * 
 * Модуль отвечает за загрузку карт из PNG файлов, управление
 * тайлами и определение областей теней. Карта состоит из
 * нескольких слоев (фон, объекты, тени).
 */

#ifndef MAP_H
#define MAP_H

#include "engine.h"

// Типы тайлов
typedef enum {
    TILE_EMPTY = 0,
    TILE_GROUND,
    TILE_WALL,
    TILE_SHADOW
} TileType;

// Структура карты
typedef struct Map {
    int width, height;          // Размеры карты в тайлах
    int tile_size;             // Размер одного тайла в пикселях
    TileType* tiles;           // Массив тайлов
    SDL_Texture* tile_textures[4]; // Текстуры для разных типов тайлов
    Vector2 world_size;        // Размер мира в пикселях
} Map;

// Функции работы с картами
Map* map_create(int width, int height, int tile_size);
void map_destroy(Map* map);

bool map_load_from_png(Map* map, SDL_Renderer* renderer, const char* png_path);
void map_set_tile(Map* map, int x, int y, TileType type);
TileType map_get_tile(const Map* map, int x, int y);

// Рендеринг карты
void map_render(Map* map, SDL_Renderer* renderer, const Camera* camera);

// Утилиты
Vector2 map_world_to_tile(const Map* map, Vector2 world_pos);
Vector2 map_tile_to_world(const Map* map, Vector2 tile_pos);
bool map_is_valid_position(const Map* map, int x, int y);
void map_create_tile_textures(Map* map, SDL_Renderer* renderer);

#endif // MAP_H
