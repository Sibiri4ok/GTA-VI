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
  // Top-left corner in world coordinates
  // World coordinates center is top-left of the map
  Vector position;
  Sprite *cur_sprite; // Current sprite to render
  bool flip_horizontal;

  // In-moment position change.
  // Needed only for convenience in movement calculations.
  // Only for dynamic objects.
  Vector velocity;
} GameObject;

Sprite load_sprite(const char *path, float scale);
void free_sprite(Sprite *sprite);

#endif
