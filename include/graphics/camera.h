#ifndef CAMERA_H
#define CAMERA_H

#include "core/engine.h"

typedef struct Camera {
    Vector2 position;
    Vector2 size;
    Vector2 target;
    Vector2 world_bounds;
    float follow_speed;
    bool following;
} Camera;

Camera* camera_create(float width, float height);
void camera_destroy(Camera* camera);

void camera_set_target(Camera* camera, Vector2 target);
void camera_set_world_bounds(Camera* camera, Vector2 bounds);
void camera_set_follow_speed(Camera* camera, float speed);
void camera_set_following(Camera* camera, bool following);

void camera_update(Camera* camera, float delta_time);
Vector2 camera_get_position(const Camera* camera);
Vector2 camera_get_size(const Camera* camera);

bool camera_is_visible(const Camera* camera, const Rect* rect);
Rect camera_get_viewport(const Camera* camera);

Vector2 camera_world_to_screen(const Camera* camera, Vector2 world_pos);
Vector2 camera_screen_to_world(const Camera* camera, Vector2 screen_pos);

#endif