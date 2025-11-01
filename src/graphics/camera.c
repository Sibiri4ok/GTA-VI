#include "graphics/camera.h"
#include <stdlib.h>

Camera *camera_create(float width, float height) {
  Camera *camera = (Camera *)calloc(1, sizeof(Camera));
  if (!camera) { return NULL; }

  camera->size = (Vector2){width, height};
  camera->position = (Vector2){0.0f, 0.0f};
  camera->target = (Vector2){0.0f, 0.0f};
  camera->follow_speed = 8.0f;
  camera->following = true;
  camera->world_bounds = (Vector2){0.0f, 0.0f};

  return camera;
}

void camera_destroy(Camera *camera) {
  if (camera) { free(camera); }
}
void camera_set_target(Camera *camera, Vector2 target) {
  if (camera) { camera->target = target; }
}

void camera_set_world_bounds(Camera *camera, Vector2 bounds) {
  if (camera) { camera->world_bounds = bounds; }
}

void camera_set_follow_speed(Camera *camera, float speed) {
  if (camera) { camera->follow_speed = speed; }
}

void camera_set_following(Camera *camera, bool following) {
  if (camera) { camera->following = following; }
}

void camera_update(Camera *camera, float delta_time) {
  if (!camera) return;

  if (camera->following) {
    // Smoothly follow the target
    Vector2 diff;
    diff.x = camera->target.x - camera->position.x;
    diff.y = camera->target.y - camera->position.y;

    float lerp = 15.0f * delta_time;
    if (lerp > 1.0f) lerp = 1.0f;

    camera->position.x += diff.x * lerp;
    camera->position.y += diff.y * lerp;
  }
}

Vector2 camera_get_position(const Camera *camera) {
  if (camera) { return camera->position; }
  return (Vector2){0.0f, 0.0f};
}

Vector2 camera_get_size(const Camera *camera) {
  if (camera) { return camera->size; }
  return (Vector2){0.0f, 0.0f};
}

bool camera_is_visible(const Camera *camera, const Rect *rect) {
  if (!camera || !rect) return false;

  Rect viewport = camera_get_viewport(camera);

  // Rectangles overlap check
  return !(rect->x + rect->w < viewport.x || rect->x > viewport.x + viewport.w ||
      rect->y + rect->h < viewport.y || rect->y > viewport.y + viewport.h);
}

Rect camera_get_viewport(const Camera *camera) {
  Rect viewport = {0, 0, 0, 0};

  if (camera) {
    viewport.x = camera->position.x - camera->size.x / 2.0f;
    viewport.y = camera->position.y - camera->size.y / 2.0f;
    viewport.w = camera->size.x;
    viewport.h = camera->size.y;
  }

  return viewport;
}

Vector2 camera_world_to_screen(const Camera *camera, Vector2 world_pos) {
  if (!camera) return world_pos;

  Vector2 screen;
  screen.x = world_pos.x - camera->position.x + camera->size.x / 2.0f;
  screen.y = world_pos.y - camera->position.y + camera->size.y / 2.0f;
  return screen;
}

Vector2 camera_screen_to_world(const Camera *camera, Vector2 screen_pos) {
  if (!camera) return screen_pos;

  Vector2 world;
  world.x = screen_pos.x + camera->position.x - camera->size.x / 2.0f;
  world.y = screen_pos.y + camera->position.y - camera->size.y / 2.0f;
  return world;
}
