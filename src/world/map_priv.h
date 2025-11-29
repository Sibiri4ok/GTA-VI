#ifndef MAP_PRIV_H
#define MAP_PRIV_H

#include "core/types_priv.h"
#include <engine/types.h>
#include <stdbool.h>
#include <stdint.h>

#define ISO_TILE_WIDTH 64
#define ISO_TILE_HEIGHT 32
#define ISO_TILE_SIDE (sqrtf(powf(ISO_TILE_HEIGHT / 2.0f, 2.0f) + powf((ISO_TILE_WIDTH / 2.0f), 2.0f)))

typedef enum { TILE_EMPTY = 0, TILE_GROUND, TILE_WATER, TILE_SAND, TILE_ROCK, TILE_GRASS, TILE_MAX } TileType;

typedef struct Map {
  uint32_t width, height;
  uint32_t width_pix, height_pix;
  uint32_t *pixels;

  TileType *tiles;
  Sprite tile_sprites[TILE_MAX];
} Map;

#endif
