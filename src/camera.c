/**
 * @file camera.c
 * @brief Реализация системы камеры и видимости
 */

#include "camera.h"
#include <math.h>
#include <stdlib.h>

Camera* camera_create(float width, float height) {
    Camera* camera = (Camera*)calloc(1, sizeof(Camera));
    if (!camera) {
        return NULL;
    }
    
    camera->size.x = width;
    camera->size.y = height;
    camera->position.x = 0.0f;
    camera->position.y = 0.0f;
    camera->target.x = 0.0f;
    camera->target.y = 0.0f;
    camera->world_bounds.x = 0.0f;
    camera->world_bounds.y = 0.0f;
    camera->follow_speed = 5.0f;
    camera->following = true;
    
    return camera;
}

void camera_destroy(Camera* camera) {
    if (camera) {
        free(camera);
    }
}

void camera_set_target(Camera* camera, Vector2 target) {
    if (camera) {
        camera->target = target;
    }
}

void camera_set_world_bounds(Camera* camera, Vector2 bounds) {
    if (camera) {
        camera->world_bounds = bounds;
    }
}

void camera_set_follow_speed(Camera* camera, float speed) {
    if (camera && speed > 0.0f) {
        camera->follow_speed = speed;
    }
}

void camera_set_following(Camera* camera, bool following) {
    if (camera) {
        camera->following = following;
    }
}

void camera_update(Camera* camera, float delta_time) {
    if (!camera || !camera->following) {
        return;
    }
    
    // Плавное следование за целью с интерполяцией
    Vector2 diff;
    diff.x = camera->target.x - camera->position.x;
    diff.y = camera->target.y - camera->position.y;
    
    float distance = sqrtf(diff.x * diff.x + diff.y * diff.y);
    if (distance > 0.5f) {
        // Используем более плавную интерполяцию
        float lerp_factor = camera->follow_speed * delta_time;
        if (lerp_factor > 1.0f) lerp_factor = 1.0f;
        
        camera->position.x += diff.x * lerp_factor;
        camera->position.y += diff.y * lerp_factor;
    } else {
        // Если очень близко, просто устанавливаем позицию
        camera->position.x = camera->target.x;
        camera->position.y = camera->target.y;
    }
    
    // Ограничиваем камеру границами мира
    float half_width = camera->size.x * 0.5f;
    float half_height = camera->size.y * 0.5f;
    
    if (camera->position.x - half_width < 0) {
        camera->position.x = half_width;
    }
    if (camera->position.x + half_width > camera->world_bounds.x) {
        camera->position.x = camera->world_bounds.x - half_width;
    }
    
    if (camera->position.y - half_height < 0) {
        camera->position.y = half_height;
    }
    if (camera->position.y + half_height > camera->world_bounds.y) {
        camera->position.y = camera->world_bounds.y - half_height;
    }
}

Vector2 camera_get_position(const Camera* camera) {
    Vector2 pos = {0, 0};
    if (camera) {
        pos = camera->position;
    }
    return pos;
}

Vector2 camera_get_size(const Camera* camera) {
    Vector2 size = {0, 0};
    if (camera) {
        size = camera->size;
    }
    return size;
}

bool camera_is_visible(const Camera* camera, const Rect* rect) {
    if (!camera || !rect) {
        return false;
    }
    
    float half_width = camera->size.x * 0.5f;
    float half_height = camera->size.y * 0.5f;
    
    float cam_left = camera->position.x - half_width;
    float cam_right = camera->position.x + half_width;
    float cam_top = camera->position.y - half_height;
    float cam_bottom = camera->position.y + half_height;
    
    return !(rect->x + rect->w < cam_left ||
             rect->x > cam_right ||
             rect->y + rect->h < cam_top ||
             rect->y > cam_bottom);
}

Rect camera_get_viewport(const Camera* camera) {
    Rect viewport = {0, 0, 0, 0};
    if (camera) {
        float half_width = camera->size.x * 0.5f;
        float half_height = camera->size.y * 0.5f;
        
        viewport.x = camera->position.x - half_width;
        viewport.y = camera->position.y - half_height;
        viewport.w = camera->size.x;
        viewport.h = camera->size.y;
    }
    return viewport;
}

Vector2 camera_world_to_screen(const Camera* camera, Vector2 world_pos) {
    Vector2 screen_pos = {0, 0};
    if (camera) {
        float half_width = camera->size.x * 0.5f;
        float half_height = camera->size.y * 0.5f;
        
        screen_pos.x = world_pos.x - (camera->position.x - half_width);
        screen_pos.y = world_pos.y - (camera->position.y - half_height);
    }
    return screen_pos;
}

Vector2 camera_screen_to_world(const Camera* camera, Vector2 screen_pos) {
    Vector2 world_pos = {0, 0};
    if (camera) {
        float half_width = camera->size.x * 0.5f;
        float half_height = camera->size.y * 0.5f;
        
        world_pos.x = screen_pos.x + (camera->position.x - half_width);
        world_pos.y = screen_pos.y + (camera->position.y - half_height);
    }
    return world_pos;
}
