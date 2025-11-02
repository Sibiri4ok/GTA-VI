#include "test_framework.h"
#include "game/map.h"

REGISTER_TEST(map_create_and_destroy) {
  Map *map = map_create(20, 15);
  TEST_ASSERT_NOT_NULL(map, "");
  TEST_ASSERT_EQ(map->width, 20, "");
  TEST_ASSERT_EQ(map->height, 15, "");
  TEST_ASSERT_NOT_NULL(map->tiles, "");
  TEST_ASSERT_NOT_NULL(map->pixels, "");
  map_destroy(map);
}

REGISTER_TEST(map_default_tiles) {
  Map *map = map_create(10, 10);
  TEST_ASSERT_NOT_NULL(map, "");
  // All tiles should be initialized to TILE_GRASS
  for (int y = 0; y < map->height; y++) {
    for (int x = 0; x < map->width; x++) {
      TEST_ASSERT_EQ(map->tiles[y * map->width + x], TILE_GRASS, "");
    }
  }
  map_destroy(map);
}

REGISTER_TEST(map_pixel_buffer_allocated) {
  Map *map = map_create(25, 25);
  TEST_ASSERT_NOT_NULL(map, "");
  TEST_ASSERT_EQ(map->width_pix, (25 + 25) * (ISO_TILE_WIDTH / 2), "");
  TEST_ASSERT_EQ(map->height_pix, (25 + 25) * (ISO_TILE_HEIGHT / 2), "");
  TEST_ASSERT_NOT_NULL(map->pixels, "");
  map_destroy(map);
}

REGISTER_TEST(map_null_handling) {
  map_destroy(NULL); // Should not crash
}
