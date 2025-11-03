#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  float x, y;
} Vector2;

typedef struct {
  uint8_t a, r, g, b;
} Color;

typedef struct {
  uint32_t *pixels;
  int width;
  int height;
} Sprite;

typedef struct {
  Vector2 position; // World coordinates
  Sprite *cur_sprite;
  Sprite *sprites; // For player it's Default, Back, Forward (for now)
  bool flip_horizontal;

  // Only for dynamic objects
  Vector2 velocity;
} GameObject;

Sprite load_sprite(const char *path, float scale);
void free_sprite(Sprite *sprite);

#endif
