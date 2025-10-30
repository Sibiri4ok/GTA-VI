

#ifndef MAP_H
#define MAP_H

#include "core/engine.h"

typedef enum {
  TILE_EMPTY = 0,
  TILE_GROUND,
  TILE_WALL,
  TILE_SHADOW,
  TILE_ROAD_NS,
  TILE_ROAD_EW,
  TILE_ROAD_CROSS,
  TILE_WATER,
  TILE_SAND,
  TILE_ROCK,
  TILE_GRASS,
  TILE_BUILDING_RESIDENTIAL,
  TILE_BUILDING_COMMERCIAL,
  TILE_BUILDING_INDUSTRIAL,
  TILE_BUILDING_PUBLIC,
  TILE_MAX
} TileType;

typedef struct Map {
  int width, height;
  int tile_width;
  int tile_height;
  TileType *tiles;
  SDL_Texture *tile_textures[TILE_MAX];
  Vector2 world_size;
  bool is_isometric;
} Map;

Map *map_create(int width, int height, int tile_size);
void map_destroy(Map *map);

void map_set_tile(Map *map, int x, int y, TileType type);
TileType map_get_tile(const Map *map, int x, int y);

void map_render(Map *map, SDL_Renderer *renderer, const Camera *camera);

Vector2 map_world_to_tile(const Map *map, Vector2 world_pos);
Vector2 map_tile_to_world(const Map *map, Vector2 tile_pos);
bool map_is_valid_position(const Map *map, int x, int y);
void map_create_tile_textures(Map *map, SDL_Renderer *renderer);

#endif
