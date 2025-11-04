#include "coordinates.h"
#include "world/map.h"

Vector2 iso_tile_to_world(int x, int y) {
  Vector2 world;
  world.x = (x - y) * (ISO_TILE_WIDTH / 2.0f);
  world.y = (x + y) * (ISO_TILE_HEIGHT / 2.0f);
  return world;
}

Vector2 iso_world_to_tile(Vector2 world_pos) {
  Vector2 tile;
  float inv_tile_w = 2.0f / ISO_TILE_WIDTH;
  float inv_tile_h = 2.0f / ISO_TILE_HEIGHT;

  tile.x = (world_pos.x * inv_tile_w + world_pos.y * inv_tile_h) / 2.0f;
  tile.y = (world_pos.y * inv_tile_h - world_pos.x * inv_tile_w) / 2.0f;
  return tile;
}
