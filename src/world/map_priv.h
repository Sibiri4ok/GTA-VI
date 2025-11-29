#ifndef MAP_PRIV_H
#define MAP_PRIV_H

#include "core/types_priv.h"
#include <engine/map.h>
#include <engine/types.h>
#include <stdbool.h>
#include <stdint.h>

#define ISO_TILE_WIDTH 64
#define ISO_TILE_HEIGHT 32

typedef struct Map {
  uint32_t width, height;
  uint32_t width_pix, height_pix;
  uint32_t *pixels;

  TilesInfo ti;
  uint32_t tile_width, tile_height;
} Map;

#endif
