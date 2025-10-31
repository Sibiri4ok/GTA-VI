#include "test_framework.h"
#include "game/map.h"

#define EPSILON 0.001f

REGISTER_TEST(map_create_and_destroy) {
  Map *map = map_create(20, 15, 64);
  TEST_ASSERT_NOT_NULL(map, "");
  TEST_ASSERT_EQ(map->width, 20, "");
  TEST_ASSERT_EQ(map->height, 15, "");
  TEST_ASSERT_EQ(map->tile_width, ISO_TILE_WIDTH, "");
  TEST_ASSERT_EQ(map->tile_height, ISO_TILE_HEIGHT, "");
  TEST_ASSERT(map->is_isometric, "");
  TEST_ASSERT_NOT_NULL(map->tiles, "");
  map_destroy(map);
}

REGISTER_TEST(map_is_valid_position) {
  Map *map = map_create(10, 10, 64);
  TEST_ASSERT_NOT_NULL(map, "");
  TEST_ASSERT(map_is_valid_position(map, 0, 0), "");
  TEST_ASSERT(map_is_valid_position(map, 9, 9), "");
  TEST_ASSERT(!map_is_valid_position(map, -1, 0), "");
  TEST_ASSERT(!map_is_valid_position(map, 10, 5), "");
  TEST_ASSERT(!map_is_valid_position(map, 5, 10), "");
  TEST_ASSERT(!map_is_valid_position(NULL, 5, 5), "");
  map_destroy(map);
}

REGISTER_TEST(map_set_and_get_tile) {
  Map *map = map_create(10, 10, 64);
  TEST_ASSERT_NOT_NULL(map, "");
  map_set_tile(map, 5, 5, TILE_GRASS);
  TEST_ASSERT_EQ(map_get_tile(map, 5, 5), TILE_GRASS, "");
  map_set_tile(map, 3, 7, TILE_WATER);
  TEST_ASSERT_EQ(map_get_tile(map, 3, 7), TILE_WATER, "");
  map_set_tile(map, 5, 5, TILE_SAND);
  TEST_ASSERT_EQ(map_get_tile(map, 5, 5), TILE_SAND, "");
  TEST_ASSERT_EQ(map_get_tile(map, -1, 5), TILE_EMPTY, "");
  TEST_ASSERT_EQ(map_get_tile(NULL, 5, 5), TILE_EMPTY, "");
  map_set_tile(NULL, 5, 5, TILE_GRASS);
  map_destroy(map);
}

REGISTER_TEST(map_default_tiles) {
  Map *map = map_create(10, 10, 64);
  TEST_ASSERT_NOT_NULL(map, "");
  for (int y = 0; y < map->height; y++) {
    for (int x = 0; x < map->width; x++) {
      TEST_ASSERT_EQ(map_get_tile(map, x, y), TILE_GRASS, "");
    }
  }
  map_destroy(map);
}

REGISTER_TEST(map_tile_to_world) {
  Map *map = map_create(10, 10, 64);
  TEST_ASSERT_NOT_NULL(map, "");
  Vector2 world = map_tile_to_world(map, (Vector2){5.0f, 3.0f});
  Vector2 expected = iso_tile_to_world(5, 3);
  TEST_ASSERT_FLOAT_EQ(world.x, expected.x, EPSILON, "");
  TEST_ASSERT_FLOAT_EQ(world.y, expected.y, EPSILON, "");
  map_destroy(map);
}

REGISTER_TEST(map_world_to_tile) {
  Map *map = map_create(10, 10, 64);
  TEST_ASSERT_NOT_NULL(map, "");
  Vector2 tile = map_world_to_tile(map, (Vector2){100.0f, 200.0f});
  Vector2 expected = iso_world_to_tile((Vector2){100.0f, 200.0f});
  TEST_ASSERT_FLOAT_EQ(tile.x, expected.x, EPSILON, "");
  TEST_ASSERT_FLOAT_EQ(tile.y, expected.y, EPSILON, "");
  map_destroy(map);
}

REGISTER_TEST(map_all_tile_types) {
  Map *map = map_create(20, 20, 64);
  TEST_ASSERT_NOT_NULL(map, "");
  TileType types[] = {TILE_EMPTY, TILE_GROUND, TILE_WALL, TILE_SHADOW, TILE_ROAD_NS,
                      TILE_ROAD_EW, TILE_ROAD_CROSS, TILE_WATER, TILE_SAND, TILE_ROCK,
                      TILE_GRASS, TILE_BUILDING_RESIDENTIAL, TILE_BUILDING_COMMERCIAL,
                      TILE_BUILDING_INDUSTRIAL, TILE_BUILDING_PUBLIC};
  for (int i = 0; i < 15; i++) {
    map_set_tile(map, i, 0, types[i]);
    TEST_ASSERT_EQ(map_get_tile(map, i, 0), types[i], "");
  }
  map_destroy(map);
}
