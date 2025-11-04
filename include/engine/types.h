#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  float x, y;
} Vector;

typedef struct {
  uint32_t x, y;
} VectorU32;

typedef struct {
  uint32_t *pixels;
  int width;
  int height;
} Sprite;

typedef struct {
  Vector position; // World coordinates
  Sprite *cur_sprite;
  bool flip_horizontal;

  // Only for dynamic objects
  Vector velocity;
} GameObject;

Sprite load_sprite(const char *path, float scale);
void free_sprite(Sprite *sprite);

#endif
