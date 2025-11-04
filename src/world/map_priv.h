#ifndef MAP_PRIV_H
#define MAP_PRIV_H

#include "core/types_priv.h"
#include <engine/types.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
  TILE_EMPTY = 0,
  TILE_GROUND,
  TILE_WATER,
  TILE_SAND,
  TILE_ROCK,
  TILE_GRASS,
  TILE_MAX
} TileType;

typedef struct Map {
  int width, height;
  int width_pix, height_pix;
  uint32_t *pixels;

  TileType *tiles;
  Sprite tile_sprites[TILE_MAX];
} Map;

#define ISO_TILE_WIDTH 64
#define ISO_TILE_HEIGHT 32

#endif
