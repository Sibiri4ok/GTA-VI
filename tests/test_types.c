#include "test_framework.h"
#include <engine/types.h>

#define EPSILON 0.001f

// Basic test: Vector creation
REGISTER_TEST(vector_basic) {
  Vector v = {10.0f, 20.0f};
  TEST_ASSERT_FLOAT_EQ(v.x, 10.0f, EPSILON, "Vector x");
  TEST_ASSERT_FLOAT_EQ(v.y, 20.0f, EPSILON, "Vector y");
}

// Basic test: Sprite loading failure
REGISTER_TEST(sprite_load_fail) {
  Sprite s = load_sprite("nonexistent.png", 1.0f);
  TEST_ASSERT_NULL(s.pixels, "Should return null pixels for missing file");
}
