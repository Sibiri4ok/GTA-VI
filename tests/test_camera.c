#include "test_framework.h"
#include "graphics/camera.h"

#define EPSILON 0.001f

REGISTER_TEST(camera_create_and_destroy) {
  Camera *camera = camera_create(800.0f, 600.0f);
  TEST_ASSERT_NOT_NULL(camera, "");
  Vector2 size = camera_get_size(camera);
  TEST_ASSERT_FLOAT_EQ(size.x, 800.0f, EPSILON, "");
  TEST_ASSERT_FLOAT_EQ(size.y, 600.0f, EPSILON, "");
  TEST_ASSERT_FLOAT_EQ(camera_get_position(camera).x, 0.0f, EPSILON, "");
  camera_destroy(camera);
}

REGISTER_TEST(camera_set_get_target) {
  Camera *camera = camera_create(800.0f, 600.0f);
  TEST_ASSERT_NOT_NULL(camera, "");
  camera_set_target(camera, (Vector2){100.0f, 200.0f});
  camera_destroy(camera);
}

REGISTER_TEST(camera_update_following) {
  Camera *camera = camera_create(800.0f, 600.0f);
  TEST_ASSERT_NOT_NULL(camera, "");
  camera_set_target(camera, (Vector2){100.0f, 200.0f});
  camera_set_following(camera, true);
  for (int i = 0; i < 100; i++) camera_update(camera, 0.016f);
  Vector2 final_pos = camera_get_position(camera);
  TEST_ASSERT(fabsf(final_pos.x - 100.0f) < 5.0f, "");
  TEST_ASSERT(fabsf(final_pos.y - 200.0f) < 5.0f, "");
  camera_destroy(camera);
}

REGISTER_TEST(camera_update_not_following) {
  Camera *camera = camera_create(800.0f, 600.0f);
  TEST_ASSERT_NOT_NULL(camera, "");
  camera->position = (Vector2){50.0f, 50.0f};
  camera_set_target(camera, (Vector2){100.0f, 200.0f});
  camera_set_following(camera, false);
  camera_update(camera, 0.016f);
  TEST_ASSERT_FLOAT_EQ(camera_get_position(camera).x, 50.0f, EPSILON, "");
  TEST_ASSERT_FLOAT_EQ(camera_get_position(camera).y, 50.0f, EPSILON, "");
  camera_destroy(camera);
}

REGISTER_TEST(camera_get_viewport) {
  Camera *camera = camera_create(800.0f, 600.0f);
  TEST_ASSERT_NOT_NULL(camera, "");
  camera->position = (Vector2){100.0f, 200.0f};
  Rect viewport = camera_get_viewport(camera);
  TEST_ASSERT_FLOAT_EQ(viewport.x, -300.0f, EPSILON, "");
  TEST_ASSERT_FLOAT_EQ(viewport.y, -100.0f, EPSILON, "");
  TEST_ASSERT_FLOAT_EQ(viewport.w, 800.0f, EPSILON, "");
  TEST_ASSERT_FLOAT_EQ(viewport.h, 600.0f, EPSILON, "");
  camera_destroy(camera);
}


REGISTER_TEST(camera_is_visible) {
  Camera *camera = camera_create(800.0f, 600.0f);
  TEST_ASSERT_NOT_NULL(camera, "");
  camera->position = (Vector2){400.0f, 300.0f};
  TEST_ASSERT(camera_is_visible(camera, &(Rect){450.0f, 350.0f, 100.0f, 100.0f}), "");
  TEST_ASSERT(!camera_is_visible(camera, &(Rect){1000.0f, 300.0f, 100.0f, 100.0f}), "");
  TEST_ASSERT(!camera_is_visible(camera, &(Rect){-200.0f, 300.0f, 100.0f, 100.0f}), "");
  TEST_ASSERT(camera_is_visible(camera, &(Rect){750.0f, 550.0f, 100.0f, 100.0f}), "");
  camera_destroy(camera);
}

REGISTER_TEST(camera_null_handling) {
  TEST_ASSERT_FLOAT_EQ(camera_get_position(NULL).x, 0.0f, EPSILON, "");
  TEST_ASSERT_FLOAT_EQ(camera_get_size(NULL).x, 0.0f, EPSILON, "");
  TEST_ASSERT_FLOAT_EQ(camera_get_viewport(NULL).x, 0.0f, EPSILON, "");
  Vector2 world = {100.0f, 200.0f};
  TEST_ASSERT_FLOAT_EQ(camera_world_to_screen(NULL, world).x, world.x, EPSILON, "");
  TEST_ASSERT_FLOAT_EQ(camera_screen_to_world(NULL, world).x, world.x, EPSILON, "");
  TEST_ASSERT(!camera_is_visible(NULL, &(Rect){100.0f, 100.0f, 50.0f, 50.0f}), "");
  camera_set_target(NULL, world);
  camera_set_world_bounds(NULL, world);
  camera_set_follow_speed(NULL, 10.0f);
  camera_set_following(NULL, true);
  camera_update(NULL, 0.016f);
  camera_destroy(NULL);
}
