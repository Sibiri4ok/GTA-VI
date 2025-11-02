#ifndef MAP_H
#define MAP_H

#include "core/types.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
  TILE_EMPTY = 0,
  TILE_GROUND,
  TILE_SHADOW,
  TILE_WATER,
  TILE_SAND,
  TILE_ROCK,
  TILE_GRASS,
  TILE_MAX
} TileType;

typedef struct Map {
  int width, height;
  int width_pix, height_pix;
  TileType *tiles;
  Sprite tile_sprites[TILE_MAX];
  uint32_t *pixels;

  GameObject *objects;
  int object_count;
} Map;

Map *map_create(int width, int height);
void map_destroy(Map *map);

#define ISO_TILE_WIDTH 64
#define ISO_TILE_HEIGHT 32

#endif
