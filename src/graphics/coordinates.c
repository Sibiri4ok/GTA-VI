#include "world/map_priv.h"
#include <engine/coordinates.h>

// Convert isometric tile coordinates to world coordinates
// Accepts isometric x, y tile coordinates and map height (in tiles)
Vector iso_tile_to_world(int x, int y, int map_height) {
  Vector world;
  // Add offset so all world x-coordinates are positive
  int offset = map_height * (ISO_TILE_WIDTH / 2);
  world.x = (x - y) * (ISO_TILE_WIDTH / 2.0f) + offset;
  world.y = (x + y) * (ISO_TILE_HEIGHT / 2.0f);
  return world;
}

// Convert world coordinates to isometric tile coordinates
// Accepts world position and map height (in tiles)
Vector iso_world_to_tile(Vector world_pos, int map_height) {
  Vector tile;
  float inv_tile_w = 2.0f / ISO_TILE_WIDTH;
  float inv_tile_h = 2.0f / ISO_TILE_HEIGHT;

  // Remove offset
  int offset = map_height * (ISO_TILE_WIDTH / 2);
  float adjusted_x = world_pos.x - offset;

  tile.x = (adjusted_x * inv_tile_w + world_pos.y * inv_tile_h) / 2.0f;
  tile.y = (world_pos.y * inv_tile_h - adjusted_x * inv_tile_w) / 2.0f;
  return tile;
}
