#include "test_framework.h"
#include <engine/map.h>

// Basic test: map creation
REGISTER_TEST(map_create_basic) {
  Map *map = map_create(50, 50);
  TEST_ASSERT_NOT_NULL(map, "Map should be created");
  map_free(map);
}
