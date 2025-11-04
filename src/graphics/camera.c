#include "camera.h"
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

void camera_free(Camera *camera) {
  if (camera) { free(camera); }
}

void camera_update(Camera *camera, float delta_time) {
  if (!camera) return;

  if (camera->following) {
    // We want to camera position (top-left corner) be shifted by half size from target
    Vector2 desired;
    desired.x = camera->target.x - camera->size.x / 2.0f;
    desired.y = camera->target.y - camera->size.y / 2.0f;

    // Smoothly follow the target
    Vector2 diff;
    diff.x = desired.x - camera->position.x;
    diff.y = desired.y - camera->position.y;

    float lerp = 15.0f * delta_time;
    if (lerp > 1.0f) lerp = 1.0f;

    camera->position.x += diff.x * lerp;
    camera->position.y += diff.y * lerp;
  }
}

bool camera_is_visible(const Camera *camera, Vector2 pos) {
  if (!camera) return false;

  return !(pos.x < 0 || pos.x >= camera->size.x || pos.y < 0 || pos.y >= camera->size.y);
}

Vector2 camera_world_to_screen(const Camera *camera, Vector2 world_pos) {
  if (!camera) return world_pos;

  Vector2 screen;
  screen.x = world_pos.x - camera->position.x;
  screen.y = world_pos.y - camera->position.y;
  return screen;
}

Vector2 camera_screen_to_world(const Camera *camera, Vector2 screen_pos) {
  if (!camera) return screen_pos;

  Vector2 world;
  world.x = screen_pos.x + camera->position.x;
  world.y = screen_pos.y + camera->position.y;
  return world;
}
