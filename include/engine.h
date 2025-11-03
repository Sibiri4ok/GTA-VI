#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>

typedef struct Engine Engine;

Engine *engine_create(int width, int height, const char *title);
void engine_destroy(Engine *e);

bool engine_begin_frame(Engine *e);
void engine_update(Engine *e);
void engine_render(Engine *e);
void engine_end_frame(Engine *e);

float engine_get_fps(Engine *e);

#endif
