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

typedef enum {
  OBJ_BUSH1 = 0,
  OBJ_BUSH2,
  OBJ_BUSH3,
  OBJ_TREE,
  OBJ_CACTUS,
  OBJ_PALM,
  OBJ_MAX
} ObjectType;

typedef struct Map {
  int width, height;
  int width_pix, height_pix;
  uint32_t *pixels;

  TileType *tiles;
  Sprite tile_sprites[TILE_MAX];

  Sprite object_sprites[OBJ_MAX];
  GameObject *st_objs;
  GameObject *dyn_objs;
  GameObject **objects; // all objects sorted by depth
} Map;

Map *map_create(int width, int height, int st_count, GameObject *dyn_objs);
void map_free(Map *map);

#define ISO_TILE_WIDTH 64
#define ISO_TILE_HEIGHT 32

#endif
