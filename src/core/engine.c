#include "core/engine.h"
#include "core/input.h"
#include "game/map.h"
#include "graphics/camera.h"
#include "graphics/display.h"
#include "graphics/render.h"
#include <graphics/load_sprite.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

#define FIXED_TIMESTEP (1.0f / 60.0f)

static int compare_objects_by_y(const void *a, const void *b) {
  GameObject *obj_a = (GameObject *)a;
  GameObject *obj_b = (GameObject *)b;

  // Sort by bottom edge of sprite for proper isometric depth
  float ya = obj_a->position.y + (obj_a->sprite ? obj_a->sprite->height : 0);
  float yb = obj_b->position.y + (obj_b->sprite ? obj_b->sprite->height : 0);

  return (ya > yb) - (ya < yb);
}

struct Engine {
  Display *display;
  Input input;

  // Map
  Map *map;

  // Player
  GameObject *player;
  Vector2 pl_velocity;

  // Player sprites
  Sprite sprite_default;
  Sprite sprite_back;
  Sprite sprite_forward;

  Camera *camera;

  // Render buffer
  uint32_t *pixels;
  int width;
  int height;

  // Object render buffer (for sorting)
  GameObject *render_objects;
  int render_objects_capacity;

  // Fixed timestep: we accumulate time between updates
  uint32_t last_frame_time;
  float accumulator;
};

Engine *engine_create(int width, int height, const char *title) {
  Engine *e = calloc(1, sizeof(Engine));
  if (!e) return NULL;

  e->width = width;
  e->height = height;

  e->display = display_create(width, height, title);
  if (!e->display) {
    free(e);
    return NULL;
  }

  e->camera = camera_create(width, height);
  if (!e->camera) {
    display_destroy(e->display);
    free(e);
    return NULL;
  }

  input_init(&e->input);

  // Allocate render buffer
  e->pixels = calloc(width * height, sizeof(uint32_t));
  if (!e->pixels) {
    camera_destroy(e->camera);
    display_destroy(e->display);
    free(e);
    return NULL;
  }

  // Create map (tile sprites are generated inside map_create)
  e->map = map_create(25, 25);
  if (!e->map) {
    free(e->pixels);
    camera_destroy(e->camera);
    display_destroy(e->display);
    free(e);
    return NULL;
  }

  float scale = 1.0f / 16.0f;
  e->sprite_default = load_sprite("assets/mario.png", scale);
  e->sprite_back = load_sprite("assets/mario_back.png", scale);
  e->sprite_forward = load_sprite("assets/mario_forward.png", scale);

  // Initialize player at center
  e->player = calloc(1, sizeof(GameObject));
  if (!e->player) {
    free(e->pixels);
    camera_destroy(e->camera);
    display_destroy(e->display);
    free(e);
    return NULL;
  }
  e->player->sprite = &e->sprite_default;
  e->player->flip_horizontal = false;

  e->player->position = (Vector2){0.0f, 0.0f};
  e->pl_velocity = (Vector2){0.0f, 0.0f};

  e->camera->target = e->player->position;

  // Allocate render buffer for all objects (map objects + player)
  e->render_objects_capacity = e->map->object_count + 1;
  e->render_objects = calloc(e->render_objects_capacity, sizeof(GameObject));
  if (!e->render_objects) {
    free(e->player);
    map_destroy(e->map);
    free(e->pixels);
    camera_destroy(e->camera);
    display_destroy(e->display);
    free(e);
    return NULL;
  }

  e->last_frame_time = display_get_ticks();
  e->accumulator = 0.0f;

  return e;
}

void engine_destroy(Engine *e) {
  if (!e) return;

  free_sprite(&e->sprite_default);
  free_sprite(&e->sprite_back);
  free_sprite(&e->sprite_forward);

  if (e->render_objects) free(e->render_objects);
  if (e->player) free(e->player);
  if (e->map) map_destroy(e->map);
  if (e->pixels) free(e->pixels);
  if (e->camera) camera_destroy(e->camera);
  if (e->display) display_destroy(e->display);
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
  e->pl_velocity.x = ax * speed;
  e->pl_velocity.y = ay * speed;
  e->player->position.x += e->pl_velocity.x;
  e->player->position.y += e->pl_velocity.y;

  // Update sprite orientation (vertical takes priority over horizontal)
  if (e->pl_velocity.y < -0.1f) {
    e->player->sprite = &e->sprite_forward;
  } else if (e->pl_velocity.y > 0.1f) {
    e->player->sprite = &e->sprite_back;
  } else if (fabs(e->pl_velocity.x) > 0.1f) {
    e->player->sprite = &e->sprite_default;
  }

  if (e->pl_velocity.x < -0.1f) {
    e->player->flip_horizontal = true;
  } else if (e->pl_velocity.x > 0.1f) {
    e->player->flip_horizontal = false;
  }

  e->camera->target = e->player->position;
  camera_update(e->camera, FIXED_TIMESTEP);
}

void engine_render(Engine *e) {
  if (!e) return;

  // Fill background
  uint32_t bg_color = 0xFF963CFF;
  for (int i = 0; i < e->width * e->height; i++) { e->pixels[i] = bg_color; }

  // Render map
  render_frame_static(e->map, e->pixels, e->camera);

  // Combine all objects for rendering (map objects + player)
  int obj_count = 0;

  // Copy map objects
  for (int i = 0; i < e->map->object_count; i++) {
    e->render_objects[obj_count++] = e->map->objects[i];
  }
  e->render_objects[obj_count++] = *e->player;

  // Sort by Y coordinate
  qsort(e->render_objects, obj_count, sizeof(GameObject), compare_objects_by_y);

  render_objects(e->pixels, e->camera, e->render_objects, obj_count);
}

void engine_end_frame(Engine *e) {
  if (!e) return;
  display_present(e->display, e->pixels);
}

float engine_get_fps(Engine *e) {
  return e ? display_get_fps(e->display) : 0.0f;
}
