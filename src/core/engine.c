#include "engine.h"
#include "core/input.h"
#include "core/types.h"
#include "game/map.h"
#include "graphics/camera.h"
#include "graphics/display.h"
#include "graphics/render.h"
#include "stb_ds.h"
#include "stb_image.h"
#include "types.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define FIXED_TIMESTEP (1.0f / 60.0f)
#define STATIC_OBJECTS_COUNT 100

struct Engine {
  Display *display;
  Input input;

  // Player sprites: default, back, forward
  GameObject *player;
  Map *map;
  Camera *camera;

  // Render buffer
  uint32_t *pixels;
  int width;
  int height;

  // Fixed timestep: we accumulate time between updates
  uint32_t last_frame_time;
  float accumulator;
};

Engine *engine_create(int width, int height, const char *title) {
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

  // Dynamic objects is only player for now
  GameObject *dyn_objs = NULL;
  arrpush(dyn_objs, (GameObject){0});
  e->player = &dyn_objs[0];

  float scale = 1.0f / 16.0f;
  e->player->sprites = NULL;
  arrpush(e->player->sprites, load_sprite("assets/mario.png", scale));
  arrpush(e->player->sprites, load_sprite("assets/mario_back.png", scale));
  arrpush(e->player->sprites, load_sprite("assets/mario_forward.png", scale));
  if (arrlen(e->player->sprites) == 0) {
    display_free(e->display);
    camera_free(e->camera);
    free(e->pixels);
    free(e);
    arrfree(dyn_objs);
    return NULL;
  }
  e->player->cur_sprite = &e->player->sprites[0];
  e->player->flip_horizontal = false;
  e->player->position = (Vector2){0.0f, 0.0f};
  e->player->velocity = (Vector2){0.0f, 0.0f};

  // Create map. Tiles and static objects are generated inside map_create
  e->map = map_create(25, 25, STATIC_OBJECTS_COUNT, dyn_objs);
  if (!e->map) {
    free(e->pixels);
    camera_free(e->camera);
    display_free(e->display);
    arrfree(dyn_objs);
    free(e);
    return NULL;
  }

  e->camera->target = e->player->position;

  e->last_frame_time = display_get_ticks();
  e->accumulator = 0.0f;

  return e;
}

void engine_free(Engine *e) {
  if (!e) return;

  // Free player sprites before freeing the map
  // TODO: Keep sprites in some place and reuse them for dynamic objs
  if (e->player && e->player->sprites) {
    for (int i = 0; i < arrlen(e->player->sprites); i++) { free_sprite(&e->player->sprites[i]); }
    arrfree(e->player->sprites);
  }

  if (e->map) map_free(e->map);
  if (e->pixels) free(e->pixels);
  if (e->camera) camera_free(e->camera);
  if (e->display) display_free(e->display);
  free(e);
}

bool engine_begin_frame(Engine *e) {
  if (!e) return false;

  printf("FPS: %d\n", (int)engine_get_fps(e));

  if (!display_poll_events(&e->input)) { return false; }

  // Fixed timestep: measure frame time
  uint32_t current_time = display_get_ticks();
  float frame_time = (current_time - e->last_frame_time) / 1000.0f;
  e->last_frame_time = current_time;

  // Cap framte time to avoid slowdown game
  if (frame_time > 0.25f) frame_time = 0.25f;

  e->accumulator += frame_time;

  // Update logic at fixed rate (60 times per second)
  while (e->accumulator >= FIXED_TIMESTEP) {
    engine_update(e);
    e->accumulator -= FIXED_TIMESTEP;
  }

  return true;
}

void engine_update(Engine *e) {
  if (!e) return;

  // Process input
  float ax = 0.0f, ay = 0.0f;
  if (e->input.a) ax -= 1.0f;
  if (e->input.d) ax += 1.0f;
  if (e->input.w) ay -= 1.0f;
  if (e->input.s) ay += 1.0f;

  // Normalize diagonal movement
  float len = sqrtf(ax * ax + ay * ay);
  if (len > 0.0f) {
    ax /= len;
    ay /= len;
  }

  // Update velocity and position
  float speed = 4.0f;
  e->player->velocity.x = ax * speed;
  e->player->velocity.y = ay * speed;
  e->player->position.x += e->player->velocity.x;
  e->player->position.y += e->player->velocity.y;

  // Update sprite orientation (vertical takes priority over horizontal)
  if (e->player->velocity.y < -0.1f) {
    e->player->cur_sprite = &e->player->sprites[2]; // Forward sprite
  } else if (e->player->velocity.y > 0.1f) {
    e->player->cur_sprite = &e->player->sprites[1]; // Back sprite
  } else if (fabs(e->player->velocity.x) > 0.1f) {
    e->player->cur_sprite = &e->player->sprites[0]; // Default sprite
  }

  if (e->player->velocity.x < -0.1f) {
    e->player->flip_horizontal = true;
  } else if (e->player->velocity.x > 0.1f) {
    e->player->flip_horizontal = false;
  }

  e->camera->target = e->player->position;
  camera_update(e->camera, FIXED_TIMESTEP);
}

void engine_render(Engine *e) {
  if (!e) return;

  // Fill background
  uint32_t bg_color = 0xFF87CEEB;
  for (int i = 0; i < e->width * e->height; i++) { e->pixels[i] = bg_color; }

  load_prerendered(e->pixels, e->map, e->camera);
  // Sort objects by depth
  qsort(e->map->objects, arrlen(e->map->objects), sizeof(GameObject *), compare_objs_by_depth);
  render_objects(e->pixels, e->map->objects, e->camera, e->map);
}

void engine_end_frame(Engine *e) {
  if (!e) return;
  display_present(e->display, e->pixels);
}

float engine_get_fps(Engine *e) {
  return e ? display_get_fps(e->display) : 0.0f;
}
