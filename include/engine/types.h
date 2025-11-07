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

  // Pointer on a custom user data
  void *data;
  // In-moment position change.
  // Needed only for convenience in movement calculations.
  // Doesn't used by engine, only by user.
  // Only for dynamic objects (you can determine which is which by yourself).
  Vector velocity;
} GameObject;

Sprite load_sprite(const char *path, float scale);
void free_sprite(Sprite *sprite);

// Load spritesheet and split into individual frames.
// Returns array of Sprite (frame_count elements).
// Frames are read left-to-right, top-to-bottom.
// Scale is applied to each frame.
Sprite *load_spritesheet_frames(const char *path,
    int frame_width,
    int frame_height,
    int frame_count,
    float scale);
void free_spritesheet_frames(Sprite *frames, int frame_count);

typedef struct {
  Vector pos;
  float w, h;
} Rect;

bool is_rect_intersect(Rect r1, Rect r2);

#endif
