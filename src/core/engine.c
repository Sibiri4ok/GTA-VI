#include "graphics/camera.h"
#include "graphics/display.h"
#include "graphics/render.h"
#include "stb_image.h"
#include "world/map_priv.h"
#include <engine/coordinates.h>
#include <engine/engine.h>
#include <engine/input.h>
#include <engine/types.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

struct Engine {
  Display *display;
  Input input;

  GameObject *player;
  Map *map;
  Camera *camera;

  // Render buffer
  uint32_t *pixels;
  int width;
  int height;

  // Fixed timestep: we accumulate time between updates
  uint64_t last_frame_time;
  float accumulator;
};

Engine *engine_create(int width, int height, const char *title) {
  if (!title || width <= 0 || height <= 0) return NULL;
  Engine *e = calloc(1, sizeof(Engine));
  if (!e) return NULL;

  e->width = width;
  e->height = height;

  e->display = display_create(width, height, 1.5f, title);
  if (!e->display) {
    free(e);
    return NULL;
  }

  e->camera = camera_create(width, height);
  if (!e->camera) {
    display_free(e->display);
    free(e);
    return NULL;
  }

  e->input = (Input){0};

  // Allocate render buffer
  e->pixels = calloc(width * height, sizeof(uint32_t));
  if (!e->pixels) {
    camera_free(e->camera);
    display_free(e->display);
    free(e);
    return NULL;
  }

  e->last_frame_time = display_get_ticks();
  e->accumulator = 0.0f;

  return e;
}

void engine_set_player(Engine *e, GameObject *player) {
  if (!e || !player) return;
  e->player = player;
  e->camera->target = e->player->position;
}

void engine_set_map(Engine *e, Map *map) {
  if (!e || !map) return;
  e->map = map;
}

void engine_free(Engine *e) {
  if (!e) return;

  if (e->display) display_free(e->display);
  if (e->camera) camera_free(e->camera);
  if (e->pixels) free(e->pixels);

  free(e);
}

// Begin frame: process input and update logic with fixed timestep
bool engine_begin_frame(Engine *e, void (*update)(Input *input, void *user_data), void *user_data) {
  if (!e) return false;

  if (!display_poll_events(&e->input)) { return false; }

  // Fixed timestep: measure frame time
  uint64_t current_time = display_get_ticks();
  float frame_time = (current_time - e->last_frame_time) / 1000.0f;

  // Cap framte time to avoid slowdown game
  if (frame_time > 0.25f) frame_time = 0.25f;

  e->accumulator += frame_time;

  // Update logic at fixed rate (60 times per second)
  while (e->accumulator >= ENGINE_LOGIC_STEP) {
    update(&e->input, user_data);
    e->accumulator -= ENGINE_LOGIC_STEP;
    e->camera->target = e->player->position;
    camera_update(e->camera, ENGINE_LOGIC_STEP);
  }

  // Update frame only after game logic updates!
  e->last_frame_time = current_time;
  return true;
}

// Render given objects on screen, sorting by depth
void engine_render(Engine *e, GameObject **objects, int count) {
  if (!e || !objects || count <= 0) return;

  // Fill background
  uint32_t bg_color = 0xFF87CEEB;
  for (int i = 0; i < e->width * e->height; i++) { e->pixels[i] = bg_color; }

  load_prerendered(e->pixels, e->map, e->camera);

  // Sort objects by depth
  qsort(objects, count, sizeof(GameObject *), compare_objs_by_depth);
  render_objects(e->pixels, objects, count, e->camera);
}

void engine_end_frame(Engine *e) {
  if (!e) return;
  display_present(e->display, e->pixels);
}

float engine_get_fps(Engine *e) {
  return e ? display_get_fps(e->display) : 0.0f;
}

// Get delta time from last frame in seconds
float engine_get_delta_time(Engine *e) {
  return e ? (display_get_ticks() - e->last_frame_time) / 1000.0f : 0.0f;
}
