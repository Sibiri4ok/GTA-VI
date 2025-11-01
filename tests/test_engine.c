#include "test_framework.h"
#include "core/engine.h"

#define EPSILON 0.001f

REGISTER_TEST(engine_iso_tile_to_world) {
  Vector2 result = iso_tile_to_world(0, 0);
  TEST_ASSERT_FLOAT_EQ(result.x, 0.0f, EPSILON, "");
  TEST_ASSERT_FLOAT_EQ(result.y, 0.0f, EPSILON, "");

  result = iso_tile_to_world(1, 0);
  TEST_ASSERT_FLOAT_EQ(result.x, (1 - 0) * (ISO_TILE_WIDTH / 2.0f), EPSILON, "");
  TEST_ASSERT_FLOAT_EQ(result.y, (1 + 0) * (ISO_TILE_HEIGHT / 2.0f), EPSILON, "");

  result = iso_tile_to_world(5, 3);
  TEST_ASSERT_FLOAT_EQ(result.x, (5 - 3) * (ISO_TILE_WIDTH / 2.0f), EPSILON, "");
  TEST_ASSERT_FLOAT_EQ(result.y, (5 + 3) * (ISO_TILE_HEIGHT / 2.0f), EPSILON, "");
}

REGISTER_TEST(engine_iso_world_to_tile) {
  Vector2 world = {0.0f, 0.0f};
  Vector2 tile = iso_world_to_tile(world);
  TEST_ASSERT_FLOAT_EQ(tile.x, 0.0f, 0.5f, "");
  TEST_ASSERT_FLOAT_EQ(tile.y, 0.0f, 0.5f, "");

  for (int x = 0; x < 10; x++) {
    for (int y = 0; y < 10; y++) {
      Vector2 original = iso_tile_to_world(x, y);
      Vector2 converted = iso_world_to_tile(original);
      TEST_ASSERT_FLOAT_EQ(converted.x, (float)x, 0.5f, "");
      TEST_ASSERT_FLOAT_EQ(converted.y, (float)y, 0.5f, "");
    }
  }
}

REGISTER_TEST(engine_iso_get_depth) {
  TEST_ASSERT_FLOAT_EQ(iso_get_depth((Vector2){0.0f, 0.0f}), 0.0f, EPSILON, "");
  TEST_ASSERT_FLOAT_EQ(iso_get_depth((Vector2){10.0f, 10.0f}), 20.0f, EPSILON, "");
  TEST_ASSERT_FLOAT_EQ(iso_get_depth((Vector2){5.0f, 3.0f}), 8.0f, EPSILON, "");
  TEST_ASSERT(iso_get_depth((Vector2){10.0f, 10.0f}) > iso_get_depth((Vector2){0.0f, 0.0f}), "");
}

REGISTER_TEST(engine_constants) {
  TEST_ASSERT_EQ(ISO_TILE_WIDTH, 64, "");
  TEST_ASSERT_EQ(ISO_TILE_HEIGHT, 32, "");
}
