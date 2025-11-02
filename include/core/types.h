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
  Sprite *sprite;
  bool flip_horizontal;
} GameObject;

#endif
