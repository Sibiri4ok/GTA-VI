#include "world/map_priv.h"
#include <engine/coordinates.h>

// Offset so all world x-coordinates in the map are positive
#define offset_x_positive(map) ((map->height - 1) * (map->tile_width / 2))

Vector tile_to_world(Map *map, int x, int y) {
  Vector world;

  int offset = offset_x_positive(map);
  world.x = (x - y) * (map->tile_width / 2.0f) + offset;
  world.y = (x + y) * (map->tile_height / 2.0f);
  return world;
}

Vector world_to_tile(Map *map, float x, float y) {
  Vector tile;
  float inv_tile_w = 2.0f / map->tile_width;
  float inv_tile_h = 2.0f / map->tile_height;

  int offset = offset_x_positive(map);
  float adjusted_x = x - offset;

  tile.x = (adjusted_x * inv_tile_w + y * inv_tile_h) / 2.0f;
  tile.y = (y * inv_tile_h - adjusted_x * inv_tile_w) / 2.0f;
  return tile;
}
