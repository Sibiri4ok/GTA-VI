#ifndef ENGINE_H
#define ENGINE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENGINE_VERSION "1.0.0"
#define TARGET_FPS 30

typedef struct Vector2 {
  float x, y;
} Vector2;

typedef struct Rect {
  float x, y, w, h;
} Rect;

typedef struct Color {
  uint8_t r, g, b, a;
} Color;

typedef struct Camera Camera;
typedef struct Renderer Renderer;
typedef struct App App;

bool engine_init(void);
void engine_cleanup(void);
void engine_update(float delta_time);
float engine_get_fps(void);
float engine_get_delta_time(void);

#define ISO_TILE_WIDTH 64
#define ISO_TILE_HEIGHT 32

Vector2 iso_world_to_screen(Vector2 world_pos);
Vector2 iso_screen_to_world(Vector2 screen_pos);
Vector2 iso_tile_to_world(int x, int y);
Vector2 iso_world_to_tile(Vector2 world_pos);
float iso_get_depth(Vector2 world_pos);

#endif
