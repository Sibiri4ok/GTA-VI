#include "world/map_priv.h"
#include <engine/coordinates.h>

Vector tile_to_world(int x, int y, int map_height) {
  Vector world;
  // Add offset so all world x-coordinates are positive
  int offset = (map_height - 1) * (ISO_TILE_WIDTH / 2);
  world.x = (x - y) * (ISO_TILE_WIDTH / 2.0f) + offset;
  world.y = (x + y) * (ISO_TILE_HEIGHT / 2.0f);
  return world;
}

Vector world_to_tile(float x, float y, int map_height) {
  Vector tile;
  float inv_tile_w = 2.0f / ISO_TILE_WIDTH;
  float inv_tile_h = 2.0f / ISO_TILE_HEIGHT;

  // Remove offset
  int offset = (map_height - 1) * (ISO_TILE_WIDTH / 2);
  float adjusted_x = x - offset;

  tile.x = (adjusted_x * inv_tile_w + y * inv_tile_h) / 2.0f;
  tile.y = (y * inv_tile_h - adjusted_x * inv_tile_w) / 2.0f;
  return tile;
}
