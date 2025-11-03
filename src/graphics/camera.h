#ifndef CAMERA_H
#define CAMERA_H

#include "core/types.h"

typedef struct Camera {
  Vector2 position;
  Vector2 size;
  Vector2 target;
  Vector2 world_bounds;
  float follow_speed;
  bool following;
} Camera;

Camera *camera_create(float width, float height);
void camera_destroy(Camera *camera);
void camera_update(Camera *camera, float delta_time);

bool camera_is_visible(const Camera *camera, Vector2 pos);

Vector2 camera_world_to_screen(const Camera *camera, Vector2 world_pos);
Vector2 camera_screen_to_world(const Camera *camera, Vector2 screen_pos);

#endif
