#include "core/engine.h"

static bool engine_initialized = false;
static float delta_time = 0.0f;
static uint32_t last_time = 0;
static float fps = 0.0f;

bool engine_init(void) {
  if (engine_initialized) { return true; }

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
    fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
    return false;
  }

  int img_flags = IMG_INIT_PNG | IMG_INIT_JPG;
  if ((IMG_Init(img_flags) & img_flags) != img_flags) {
    fprintf(stderr, "SDL_image Init Error: %s\n", IMG_GetError());
    SDL_Quit();
    return false;
  }

  engine_initialized = true;
  last_time = SDL_GetTicks();

  return true;
}

void engine_cleanup(void) {
  if (!engine_initialized) { return; }

  IMG_Quit();
  SDL_Quit();

  engine_initialized = false;
}

void engine_update(float dt) {
  delta_time = dt;

  uint32_t current_time = SDL_GetTicks();
  uint32_t elapsed = current_time - last_time;
  if (elapsed > 0) { fps = 1000.0f / (float)elapsed; }
  last_time = current_time;
}

float engine_get_fps(void) {
  return fps;
}

float engine_get_delta_time(void) {
  return delta_time;
}

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

Vector2 iso_world_to_screen(Vector2 world_pos) {
  Vector2 screen;
  screen.x = world_pos.x + 400.0f;
  screen.y = world_pos.y + 300.0f;
  return screen;
}

Vector2 iso_screen_to_world(Vector2 screen_pos) {
  Vector2 world;
  world.x = screen_pos.x - 400.0f;
  world.y = screen_pos.y - 300.0f;
  return world;
}

float iso_get_depth(Vector2 world_pos) {
  return world_pos.x + world_pos.y;
}
