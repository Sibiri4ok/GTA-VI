#ifndef CAMERA_H
#define CAMERA_H

#include <engine/types.h>

typedef struct Camera {
  Vector position; // top-left corner in world coordinates
  Vector size;
  Vector target;
  Vector world_bounds;
  float follow_speed;
  bool following;
} Camera;

Camera *camera_create(float width, float height);
void camera_free(Camera *camera);
void camera_update(Camera *camera, float delta_time);

bool camera_is_visible(const Camera *camera, Vector pos);

Vector camera_world_to_screen(const Camera *camera, Vector world_pos);
Vector camera_screen_to_world(const Camera *camera, Vector screen_pos);

#endif
