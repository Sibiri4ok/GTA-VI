#include "test_framework.h"
#include <engine/coordinates.h>

#define EPSILON 0.5f

// Basic test: round-trip conversion
REGISTER_TEST(coordinates_round_trip) {
  int map_height = 50;
  Vector world = iso_tile_to_world(5, 5, map_height);
  Vector tile = iso_world_to_tile(world, map_height);

  TEST_ASSERT_FLOAT_EQ(tile.x, 5.0f, EPSILON, "Tile X after round-trip");
  TEST_ASSERT_FLOAT_EQ(tile.y, 5.0f, EPSILON, "Tile Y after round-trip");
}
