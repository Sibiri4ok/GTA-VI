#include "core/engine.h"
#include "core/input.h"
#include "game/map.h"
#include "graphics/camera.h"
#include "graphics/display.h"
#include "graphics/render.h"
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
  uint32_t *map_buffer;

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

  // Camera
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

  // Create camera
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

  // Allocate map buffer for pre-rendered static map
  e->map_buffer = calloc(width * height, sizeof(uint32_t));
  if (!e->map_buffer) {
    free(e->pixels);
    camera_destroy(e->camera);
    display_destroy(e->display);
    free(e);
    return NULL;
  }

  // Create map
  e->map = map_create(25, 25, ISO_TILE_WIDTH);
  if (!e->map) {
    free(e->map_buffer);
    free(e->pixels);
    camera_destroy(e->camera);
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
  if (e->map_buffer) free(e->map_buffer);
  if (e->pixels) free(e->pixels);
  if (e->camera) camera_destroy(e->camera);
  if (e->display) display_destroy(e->display);
  free(e);
}

bool engine_begin_frame(Engine *e) {
  if (!e) return false;

  printf("FPS: %.2f\n", engine_get_fps(e));

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

  float speed = 4.0f;
  e->player_velocity.x = ax * speed;
  e->player_velocity.y = ay * speed;

  e->player_position.x += e->player_velocity.x;
  e->player_position.y += e->player_velocity.y;

  // Save current orientation when moving
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

  // Update camera to follow player
  camera_set_target(e->camera, e->player_position);
  camera_update(e->camera, FIXED_TIMESTEP);
}

void engine_render(Engine *e) {
  if (!e) return;

  // Fill background
  uint32_t bg_color = 0xFF963CFF;
  for (int i = 0; i < e->width * e->height; i++) { e->pixels[i] = bg_color; }

  // Render map with camera
  Vector2 cam_pos = camera_get_position(e->camera);
  map_render_software(e->map, e->pixels, e->width, e->height, cam_pos);

  // Draw player sprite
  Sprite *current_sprite = &e->sprite_default;
  bool flip_horizontal = false;

  // Check if player is moving
  float vel_mag = sqrtf(
      e->player_velocity.x * e->player_velocity.x + e->player_velocity.y * e->player_velocity.y);

  if (vel_mag > 0.1f) {
    // Player is moving - determine sprite based on velocity
    if (e->player_velocity.y < -0.1f) {
      current_sprite = &e->sprite_forward;
    } else if (e->player_velocity.y > 0.1f) {
      current_sprite = &e->sprite_back;
    }
    flip_horizontal = e->player_velocity.x < -0.1f;
  } else {
    // Player is idle - use last known orientation
    current_sprite = e->last_sprite;
    flip_horizontal = e->last_flip_horizontal;
  }

  // Convert player world position to screen position
  Vector2 player_screen = camera_world_to_screen(e->camera, e->player_position);
  int player_screen_x = (int)player_screen.x;
  int player_screen_y = (int)player_screen.y;

  if (current_sprite->pixels) {
    draw_sprite(e->pixels,
        e->width,
        e->height,
        current_sprite,
        player_screen_x - current_sprite->width / 2,
        player_screen_y - current_sprite->height / 2,
        flip_horizontal);
  }
}

void engine_end_frame(Engine *e) {
  if (!e) return;
  display_present(e->display, e->pixels);
}

float engine_get_fps(Engine *e) {
  return e ? display_get_fps(e->display) : 0.0f;
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
