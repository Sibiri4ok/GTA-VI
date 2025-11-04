#ifndef ENGINE_H
#define ENGINE_H

#include <engine/input.h>
#include <engine/map.h>
#include <engine/types.h>
#include <stdbool.h>

typedef struct Engine Engine;

Engine *engine_create(int width, int height, const char *title);
void engine_set_player(Engine *e, GameObject *player);
void engine_set_map(Engine *e, Map *map);
void engine_free(Engine *e);

bool engine_begin_frame(Engine *e, void (*update)(Input *input, void *user_data), void *user_data);
void engine_render(Engine *e, GameObject **objects, int count);
void engine_end_frame(Engine *e);

float engine_get_fps(Engine *e);
uint64_t engine_get_time(Engine *e);

#endif
