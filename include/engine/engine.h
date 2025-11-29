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

// Begin frame: process input and update logic with fixed timestep.
//
// 'update' is a pointer to user-defined function that process pressed buttons and updates game logic.
// 'user_data' is a pointer to user data that will be passed to 'update' function.
// Returns false if the user requested to quit the application (pressed Esc or Close button).
bool engine_begin_frame(Engine *e, void (*update)(Input *input, void *user_data), void *user_data);
// Render given batch on screen, sorting by depth
void engine_render(Engine *e, RenderBatch *batch);
// End frame: present rendered frame on screen and update FPS.
void engine_end_frame(Engine *e);

// Get current FPS. Calculation based on the EMA (Exponential Moving Average) formula.
float engine_get_fps(Engine *e);
// Time between last two displayed frames in milliseconds.
uint64_t engine_get_delta_time(Engine *e);

#endif
