/**
 * @file camera.h
 * @brief Система камеры и видимости
 * 
 * Модуль управляет камерой, которая следит за персонажем и определяет,
 * какие объекты попадают в зону видимости. Камера не выходит за границы
 * карты и обеспечивает плавное следование за целью.
 */

#ifndef CAMERA_H
#define CAMERA_H

#include "engine.h"

// Структура камеры
typedef struct Camera {
    Vector2 position;        // Позиция камеры в мире
    Vector2 size;           // Размер области видимости
    Vector2 target;         // Цель для следования
    Vector2 world_bounds;   // Границы мира (максимальные координаты)
    float follow_speed;     // Скорость следования за целью
    bool following;         // Следует ли за целью
} Camera;

// Функции работы с камерой
Camera* camera_create(float width, float height);
void camera_destroy(Camera* camera);

void camera_set_target(Camera* camera, Vector2 target);
void camera_set_world_bounds(Camera* camera, Vector2 bounds);
void camera_set_follow_speed(Camera* camera, float speed);
void camera_set_following(Camera* camera, bool following);

void camera_update(Camera* camera, float delta_time);
Vector2 camera_get_position(const Camera* camera);
Vector2 camera_get_size(const Camera* camera);

// Проверка видимости объекта
bool camera_is_visible(const Camera* camera, const Rect* rect);
Rect camera_get_viewport(const Camera* camera);

// Преобразование координат
Vector2 camera_world_to_screen(const Camera* camera, Vector2 world_pos);
Vector2 camera_screen_to_world(const Camera* camera, Vector2 screen_pos);

#endif // CAMERA_H
