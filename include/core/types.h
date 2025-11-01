#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef struct {
  float x, y;
} Vector2;

typedef struct {
  uint8_t r, g, b, a;
} Color;

typedef struct {
  uint32_t *pixels;
  int width;
  int height;
} Sprite;

typedef struct {
  float x, y, w, h;
} Rect;

#define ISO_TILE_WIDTH 64
#define ISO_TILE_HEIGHT 32

#endif
