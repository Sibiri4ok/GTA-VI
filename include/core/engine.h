#ifndef ENGINE_H
#define ENGINE_H

#include "core/types.h"
#include <stdbool.h>

typedef struct Engine Engine;

Engine *engine_create(int width, int height, const char *title);
void engine_destroy(Engine *e);

bool engine_begin_frame(Engine *e);
void engine_update(Engine *e);
void engine_render(Engine *e);
void engine_end_frame(Engine *e);

float engine_get_fps(Engine *e);
float engine_get_delta_time(Engine *e);

// Isometric utilities
Vector2 iso_tile_to_world(int x, int y);
Vector2 iso_world_to_tile(Vector2 world_pos);
float iso_get_depth(Vector2 world_pos);

#endif
