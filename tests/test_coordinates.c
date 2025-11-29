#include "test_framework.h"
#include <engine/coordinates.h>
#include <engine/map.h>
#include <engine/types.h>

#define MAP_WIDTH 50
#define MAP_HEIGHT 50
#define EPSILON 0.5f

// Basic test: round-trip conversion
REGISTER_TEST(coordinates_round_trip) {
  TilesInfo ti = {0};
  ti.tile_sprites = calloc(1, sizeof(Sprite));
  ti.tile_sprites[0] = load_sprite("assets/grass_high.png", 1.0f / 7.2f);
  ti.sprite_count = 1;
  ti.tiles = calloc(MAP_WIDTH * MAP_HEIGHT, sizeof(uint32_t));
  ti.sides_height = 64;
  Map *map = map_create(MAP_WIDTH, MAP_HEIGHT, ti);
  if (!map) { TEST_ASSERT(false, "Failed to create map for coordinate test"); };

  Vector world = tile_to_world(map, 5, 5);
  Vector tile = world_to_tile(map, world.x, world.y);

  TEST_ASSERT_FLOAT_EQ(tile.x, 5.0f, EPSILON, "Tile X after round-trip");
  TEST_ASSERT_FLOAT_EQ(tile.y, 5.0f, EPSILON, "Tile Y after round-trip");
}
