#ifndef ENGINE_H
#define ENGINE_H

#include <engine/input.h>
#include <engine/map.h>
#include <engine/types.h>
#include <stdbool.h>

// Fixed timestep for game logic updates (60 updates per second).
// Game logic speed is independent of FPS.
#define ENGINE_LOGIC_STEP (1.0f / 60.0f)

typedef struct Engine Engine;

Engine *engine_create(int width, int height, const char *title);
void engine_set_player(Engine *e, GameObject *player);
void engine_set_map(Engine *e, Map *map);
void engine_free(Engine *e);

bool engine_begin_frame(Engine *e, void (*update)(Input *input, void *user_data), void *user_data);
void engine_render(Engine *e, GameObject **objects, int count);
void engine_end_frame(Engine *e);

// Get current FPS. Calculation based on a last frame time.
float engine_get_fps(Engine *e);
// Time from last frame in seconds.
float engine_get_delta_time(Engine *e);

#endif
