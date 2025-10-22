/**
 * @file map.c
 * @brief Реализация системы загрузки и управления картами
 */

#include "map.h"
#include "camera.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Map* map_create(int width, int height, int tile_size) {
    if (width <= 0 || height <= 0 || tile_size <= 0) {
        return NULL;
    }
    
    Map* map = (Map*)calloc(1, sizeof(Map));
    if (!map) {
        return NULL;
    }
    
    map->width = width;
    map->height = height;
    map->tile_size = tile_size;
    map->world_size.x = (float)(width * tile_size);
    map->world_size.y = (float)(height * tile_size);
    
    // Выделяем память для тайлов
    map->tiles = (TileType*)calloc(width * height, sizeof(TileType));
    if (!map->tiles) {
        free(map);
        return NULL;
    }
    
    // Инициализируем текстуры
    for (int i = 0; i < 4; i++) {
        map->tile_textures[i] = NULL;
    }
    
    return map;
}

void map_destroy(Map* map) {
    if (!map) {
        return;
    }
    
    // Освобождаем текстуры
    for (int i = 0; i < 4; i++) {
        if (map->tile_textures[i]) {
            SDL_DestroyTexture(map->tile_textures[i]);
        }
    }
    
    // Освобождаем память
    if (map->tiles) {
        free(map->tiles);
    }
    
    free(map);
}

bool map_load_from_png(Map* map, SDL_Renderer* renderer, const char* png_path) {
    if (!map || !renderer || !png_path) {
        return false;
    }
    
    // Загружаем изображение
    SDL_Surface* surface = IMG_Load(png_path);
    if (!surface) {
        printf("Ошибка загрузки карты %s: %s\n", png_path, IMG_GetError());
        return false;
    }
    
    // Проверяем размеры
    if (surface->w != map->width || surface->h != map->height) {
        printf("Размеры карты не совпадают: ожидается %dx%d, получено %dx%d\n",
               map->width, map->height, surface->w, surface->h);
        SDL_FreeSurface(surface);
        return false;
    }
    
    // Конвертируем в формат с альфа-каналом
    SDL_Surface* converted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_FreeSurface(surface);
    
    if (!converted) {
        printf("Ошибка конвертации поверхности: %s\n", SDL_GetError());
        return false;
    }
    
    // Читаем пиксели и создаем тайлы
    uint32_t* pixels = (uint32_t*)converted->pixels;
    int pitch = converted->pitch / sizeof(uint32_t);
    
    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            uint32_t pixel = pixels[y * pitch + x];
            uint8_t r = (pixel >> 24) & 0xFF;
            uint8_t g = (pixel >> 16) & 0xFF;
            uint8_t b = (pixel >> 8) & 0xFF;
            uint8_t a = pixel & 0xFF;
            
            // Определяем тип тайла по цвету
            TileType type = TILE_EMPTY;
            if (a < 128) {
                type = TILE_EMPTY;  // Прозрачный
            } else if (r > 200 && g > 200 && b > 200) {
                type = TILE_GROUND;  // Белый/светлый - земля
            } else if (r < 100 && g < 100 && b < 100) {
                type = TILE_SHADOW;  // Темный - тень
            } else {
                type = TILE_WALL;   // Остальное - стены
            }
            
            map_set_tile(map, x, y, type);
        }
    }
    
    SDL_FreeSurface(converted);
    
    // Создаем простые текстуры для тайлов
    map_create_tile_textures(map, renderer);
    
    return true;
}

void map_create_tile_textures(Map* map, SDL_Renderer* renderer) {
    // Создаем простые цветные текстуры для каждого типа тайла
    SDL_Surface* surfaces[4];
    
    // Пустой тайл (прозрачный)
    surfaces[TILE_EMPTY] = SDL_CreateRGBSurface(0, map->tile_size, map->tile_size, 32, 0, 0, 0, 0);
    SDL_FillRect(surfaces[TILE_EMPTY], NULL, SDL_MapRGBA(surfaces[TILE_EMPTY]->format, 0, 0, 0, 0));
    
    // Земля (зеленый)
    surfaces[TILE_GROUND] = SDL_CreateRGBSurface(0, map->tile_size, map->tile_size, 32, 0, 0, 0, 0);
    SDL_FillRect(surfaces[TILE_GROUND], NULL, SDL_MapRGBA(surfaces[TILE_GROUND]->format, 100, 200, 100, 255));
    
    // Стена (коричневый)
    surfaces[TILE_WALL] = SDL_CreateRGBSurface(0, map->tile_size, map->tile_size, 32, 0, 0, 0, 0);
    SDL_FillRect(surfaces[TILE_WALL], NULL, SDL_MapRGBA(surfaces[TILE_WALL]->format, 139, 69, 19, 255));
    
    // Тень (темно-серый)
    surfaces[TILE_SHADOW] = SDL_CreateRGBSurface(0, map->tile_size, map->tile_size, 32, 0, 0, 0, 0);
    SDL_FillRect(surfaces[TILE_SHADOW], NULL, SDL_MapRGBA(surfaces[TILE_SHADOW]->format, 50, 50, 50, 255));
    
    // Создаем текстуры
    for (int i = 0; i < 4; i++) {
        if (surfaces[i]) {
            map->tile_textures[i] = SDL_CreateTextureFromSurface(renderer, surfaces[i]);
            SDL_FreeSurface(surfaces[i]);
        }
    }
}

void map_set_tile(Map* map, int x, int y, TileType type) {
    if (!map || x < 0 || x >= map->width || y < 0 || y >= map->height) {
        return;
    }
    
    map->tiles[y * map->width + x] = type;
}

TileType map_get_tile(const Map* map, int x, int y) {
    if (!map || x < 0 || x >= map->width || y < 0 || y >= map->height) {
        return TILE_EMPTY;
    }
    
    return map->tiles[y * map->width + x];
}

void map_render(Map* map, SDL_Renderer* renderer, const Camera* camera) {
    if (!map || !renderer || !camera) {
        return;
    }
    
    // Получаем область видимости камеры
    Rect viewport = camera_get_viewport(camera);
    
    // Определяем какие тайлы нужно рендерить с запасом для устранения зазоров
    int start_x = (int)(viewport.x / map->tile_size) - 2;
    int start_y = (int)(viewport.y / map->tile_size) - 2;
    int end_x = (int)((viewport.x + viewport.w) / map->tile_size) + 2;
    int end_y = (int)((viewport.y + viewport.h) / map->tile_size) + 2;
    
    // Ограничиваем границами карты
    if (start_x < 0) start_x = 0;
    if (start_y < 0) start_y = 0;
    if (end_x >= map->width) end_x = map->width - 1;
    if (end_y >= map->height) end_y = map->height - 1;
    
    // Рендерим видимые тайлы
    for (int y = start_y; y <= end_y; y++) {
        for (int x = start_x; x <= end_x; x++) {
            TileType type = map_get_tile(map, x, y);
            if (type == TILE_EMPTY || !map->tile_textures[type]) {
                continue;
            }
            
            // Вычисляем позицию тайла на экране
            Vector2 world_pos = {(float)(x * map->tile_size), (float)(y * map->tile_size)};
            Vector2 screen_pos = camera_world_to_screen(camera, world_pos);
            
            // Проверяем, что тайл попадает в область экрана с небольшим запасом
            if (screen_pos.x + map->tile_size < -map->tile_size || screen_pos.x > viewport.w + map->tile_size ||
                screen_pos.y + map->tile_size < -map->tile_size || screen_pos.y > viewport.h + map->tile_size) {
                continue;
            }
            
            SDL_Rect dest_rect = {
                (int)screen_pos.x,
                (int)screen_pos.y,
                map->tile_size,
                map->tile_size
            };
            
            SDL_RenderCopy(renderer, map->tile_textures[type], NULL, &dest_rect);
        }
    }
}

Vector2 map_world_to_tile(const Map* map, Vector2 world_pos) {
    Vector2 tile_pos = {0, 0};
    if (map) {
        tile_pos.x = world_pos.x / map->tile_size;
        tile_pos.y = world_pos.y / map->tile_size;
    }
    return tile_pos;
}

Vector2 map_tile_to_world(const Map* map, Vector2 tile_pos) {
    Vector2 world_pos = {0, 0};
    if (map) {
        world_pos.x = tile_pos.x * map->tile_size;
        world_pos.y = tile_pos.y * map->tile_size;
    }
    return world_pos;
}

bool map_is_valid_position(const Map* map, int x, int y) {
    return map && x >= 0 && x < map->width && y >= 0 && y < map->height;
}
