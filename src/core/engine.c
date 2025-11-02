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

struct Engine {
  Display *display;
  Input input;

  // Map
  Map *map;

  // Player
  Vector2 player_position;
  Vector2 player_velocity;

  // Player sprites
  Sprite sprite_default;
  Sprite sprite_back;
  Sprite sprite_forward;

  // Player orientation (to preserve direction when idle)
  Sprite *last_sprite;
  bool last_flip_horizontal;

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
  e->player_position = (Vector2){0.0f, 0.0f};
  e->player_velocity = (Vector2){0.0f, 0.0f};

  // Initialize player orientation (facing right by default)
  e->last_sprite = &e->sprite_default;
  e->last_flip_horizontal = false;

  camera_set_target(e->camera, e->player_position);

  e->last_frame_time = display_get_ticks();
  e->accumulator = 0.0f;

  return e;
}

void engine_destroy(Engine *e) {
  if (!e) return;

  free_sprite(&e->sprite_default);
  free_sprite(&e->sprite_back);
  free_sprite(&e->sprite_forward);

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
  e->player_velocity.x = ax * speed;
  e->player_velocity.y = ay * speed;
  e->player_position.x += e->player_velocity.x;
  e->player_position.y += e->player_velocity.y;

  // Update sprite orientation (vertical takes priority over horizontal)
  if (e->player_velocity.y < -0.1f) {
    e->last_sprite = &e->sprite_forward;
  } else if (e->player_velocity.y > 0.1f) {
    e->last_sprite = &e->sprite_back;
  } else if (fabs(e->player_velocity.x) > 0.1f) {
    e->last_sprite = &e->sprite_default;
  }

  if (e->player_velocity.x < -0.1f) {
    e->last_flip_horizontal = true;
  } else if (e->player_velocity.x > 0.1f) {
    e->last_flip_horizontal = false;
  }

  camera_set_target(e->camera, e->player_position);
  camera_update(e->camera, FIXED_TIMESTEP);
}

void engine_render(Engine *e) {
  if (!e) return;

  // Fill background
  uint32_t bg_color = 0xFF963CFF;
  for (int i = 0; i < e->width * e->height; i++) { e->pixels[i] = bg_color; }

  // Render map
  Vector2 cam_pos = camera_get_position(e->camera);
  render_frame_static(e->map, e->pixels, e->width, e->height, cam_pos);

  // Draw player (sprite orientation determined in engine_update)
  Vector2 player_screen = camera_world_to_screen(e->camera, e->player_position);

  if (e->last_sprite->pixels) {
    draw_sprite(e->pixels,
        e->width,
        e->height,
        e->last_sprite,
        (int)player_screen.x - e->last_sprite->width / 2,
        (int)player_screen.y - e->last_sprite->height / 2,
        e->last_flip_horizontal);
  }
}

void engine_end_frame(Engine *e) {
  if (!e) return;
  display_present(e->display, e->pixels);
}

float engine_get_fps(Engine *e) {
  return e ? display_get_fps(e->display) : 0.0f;
}
