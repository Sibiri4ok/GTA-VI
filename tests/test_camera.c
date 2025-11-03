#include "graphics/camera.h"
#include "test_framework.h"
#include <math.h>

#define EPSILON 0.001f

REGISTER_TEST(camera_create_and_destroy) {
  Camera *camera = camera_create(800.0f, 600.0f);
  TEST_ASSERT_NOT_NULL(camera, "");
  TEST_ASSERT_FLOAT_EQ(camera->size.x, 800.0f, EPSILON, "");
  TEST_ASSERT_FLOAT_EQ(camera->size.y, 600.0f, EPSILON, "");
  TEST_ASSERT_FLOAT_EQ(camera->position.x, 0.0f, EPSILON, "");
  camera_free(camera);
}

REGISTER_TEST(camera_update_following) {
  Camera *camera = camera_create(800.0f, 600.0f);
  TEST_ASSERT_NOT_NULL(camera, "");
  camera->target = (Vector2){100.0f, 200.0f};
  camera->following = true;
  for (int i = 0; i < 100; i++) camera_update(camera, 0.016f);
  TEST_ASSERT(fabsf(camera->position.x - 100.0f) < 5.0f, "");
  TEST_ASSERT(fabsf(camera->position.y - 200.0f) < 5.0f, "");
  camera_free(camera);
}

REGISTER_TEST(camera_update_not_following) {
  Camera *camera = camera_create(800.0f, 600.0f);
  TEST_ASSERT_NOT_NULL(camera, "");
  camera->position = (Vector2){50.0f, 50.0f};
  camera->target = (Vector2){100.0f, 200.0f};
  camera->following = false;
  camera_update(camera, 0.016f);
  TEST_ASSERT_FLOAT_EQ(camera->position.x, 50.0f, EPSILON, "");
  TEST_ASSERT_FLOAT_EQ(camera->position.y, 50.0f, EPSILON, "");
  camera_free(camera);
}

REGISTER_TEST(camera_is_visible) {
  Camera *camera = camera_create(800.0f, 600.0f);
  TEST_ASSERT_NOT_NULL(camera, "");
  camera->position = (Vector2){400.0f, 300.0f};
  TEST_ASSERT(camera_is_visible(camera, (Vector2){450.0f, 350.0f}), "");
  TEST_ASSERT(!camera_is_visible(camera, (Vector2){1000.0f, 300.0f}), "");
  TEST_ASSERT(!camera_is_visible(camera, (Vector2){-200.0f, 300.0f}), "");
  camera_free(camera);
}

REGISTER_TEST(camera_world_screen_conversion) {
  Camera *camera = camera_create(800.0f, 600.0f);
  TEST_ASSERT_NOT_NULL(camera, "");
  camera->position = (Vector2){400.0f, 300.0f};

  Vector2 world = {500.0f, 400.0f};
  Vector2 screen = camera_world_to_screen(camera, world);
  Vector2 back = camera_screen_to_world(camera, screen);

  TEST_ASSERT_FLOAT_EQ(back.x, world.x, EPSILON, "");
  TEST_ASSERT_FLOAT_EQ(back.y, world.y, EPSILON, "");
  camera_free(camera);
}

REGISTER_TEST(camera_null_handling) {
  Vector2 world = {100.0f, 200.0f};
  TEST_ASSERT(!camera_is_visible(NULL, world), "");
  camera_update(NULL, 0.016f);
  camera_free(NULL);
}
