#include "core/engine.h"
#include "core/input.h"
#include "game/map.h"
#include "graphics/display.h"
#include "graphics/render.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

struct Engine {
  Display *display;
  Input input;

  // Map
  Map *map;
  uint32_t *map_buffer;

  // Player
  Vector2 player_position;
  Vector2 player_velocity;

  // Player sprites
  Sprite sprite_default;
  Sprite sprite_back;
  Sprite sprite_forward;

  // Camera
  Vector2 camera_position;

  // Render buffer
  uint32_t *pixels;
  int width;
  int height;

  // Timing
  uint32_t last_frame_time;
  float delta_time;
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

  input_init(&e->input);

  // Allocate render buffer
  e->pixels = calloc(width * height, sizeof(uint32_t));
  if (!e->pixels) {
    display_destroy(e->display);
    free(e);
    return NULL;
  }

  // Allocate map buffer for pre-rendered static map
  e->map_buffer = calloc(width * height, sizeof(uint32_t));
  if (!e->map_buffer) {
    free(e->pixels);
    display_destroy(e->display);
    free(e);
    return NULL;
  }

  // Create map
  e->map = map_create(20, 20, ISO_TILE_WIDTH);
  if (!e->map) {
    free(e->map_buffer);
    free(e->pixels);
    display_destroy(e->display);
    free(e);
    return NULL;
  }

  // Generate procedural tile sprites
  map_create_tile_sprites(e->map);

  // Load player sprites (scaled down 16x)
  e->sprite_default = load_sprite_scaled("assets/mario.png", 16);
  e->sprite_back = load_sprite_scaled("assets/mario_back.png", 16);
  e->sprite_forward = load_sprite_scaled("assets/mario_forward.png", 16);

  // Initialize player at center
  e->player_position = (Vector2){0.0f, 0.0f};
  e->player_velocity = (Vector2){0.0f, 0.0f};

  // Camera at player
  e->camera_position = e->player_position;

  e->last_frame_time = 0;
  e->delta_time = 0.016f;

  return e;
}

void engine_destroy(Engine *e) {
  if (!e) return;

  free_sprite(&e->sprite_default);
  free_sprite(&e->sprite_back);
  free_sprite(&e->sprite_forward);

  if (e->map) map_destroy(e->map);
  if (e->map_buffer) free(e->map_buffer);
  if (e->pixels) free(e->pixels);
  if (e->display) display_destroy(e->display);
  free(e);
}

bool engine_begin_frame(Engine *e) {
  if (!e) return false;

  // Poll events
  if (!display_poll_events(&e->input)) { return false; }

  return true;
}

void engine_update(Engine *e) {
  if (!e) return;

  // keys are held-down
  float ax = 0.0f, ay = 0.0f;
  if (e->input.a) ax -= 1.0f;
  if (e->input.d) ax += 1.0f;
  if (e->input.w) ay -= 1.0f;
  if (e->input.s) ay += 1.0f;

  // Normalization (to diagonal movespeed was not faster)
  float len = sqrtf(ax * ax + ay * ay);
  if (len > 0.0f) {
    ax /= len;
    ay /= len;
  }

  float speed = 20.0f;
  e->player_velocity.x = ax * speed;
  e->player_velocity.y = ay * speed;

  e->player_position.x += e->player_velocity.x * e->delta_time;
  e->player_position.y += e->player_velocity.y * e->delta_time;

  e->camera_position = e->player_position;
}

void engine_render(Engine *e) {
  if (!e) return;

  // Fill background
  uint32_t bg_color = 0xFF963CFF;
  for (int i = 0; i < e->width * e->height; i++) { e->pixels[i] = bg_color; }

  // Render map with camera
  map_render_software(e->map, e->pixels, e->width, e->height, e->camera_position);

  // Draw player sprite
  Sprite *current_sprite = &e->sprite_default;
  if (e->player_velocity.y < -0.1f) {
    current_sprite = &e->sprite_forward;
  } else if (e->player_velocity.y > 0.1f) {
    current_sprite = &e->sprite_back;
  }

  // Convert player world position to screen position
  int player_screen_x = (int)(e->player_position.x - e->camera_position.x + e->width / 2);
  int player_screen_y = (int)(e->player_position.y - e->camera_position.y + e->height / 2);

  if (current_sprite->pixels) {
    draw_sprite(e->pixels,
        e->width,
        e->height,
        current_sprite,
        player_screen_x - current_sprite->width / 2,
        player_screen_y - current_sprite->height / 2);
  }
}

void engine_end_frame(Engine *e) {
  if (!e) return;
  display_present(e->display, e->pixels);
}

float engine_get_fps(Engine *e) {
  return e ? display_get_fps(e->display) : 0.0f;
}

float engine_get_delta_time(Engine *e) {
  return e ? e->delta_time : 0.0f;
}

// Isometric utilities
Vector2 iso_tile_to_world(int x, int y) {
  Vector2 world;
  world.x = (x - y) * (ISO_TILE_WIDTH / 2.0f);
  world.y = (x + y) * (ISO_TILE_HEIGHT / 2.0f);
  return world;
}

Vector2 iso_world_to_tile(Vector2 world_pos) {
  Vector2 tile;
  float inv_tile_w = 2.0f / ISO_TILE_WIDTH;
  float inv_tile_h = 2.0f / ISO_TILE_HEIGHT;

  tile.x = (world_pos.x * inv_tile_w + world_pos.y * inv_tile_h) / 2.0f;
  tile.y = (world_pos.y * inv_tile_h - world_pos.x * inv_tile_w) / 2.0f;
  return tile;
}

float iso_get_depth(Vector2 world_pos) {
  return world_pos.x + world_pos.y;
}
