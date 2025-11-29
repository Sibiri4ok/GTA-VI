#ifndef TYPES_H
#define TYPES_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
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
  uint32_t width;
  uint32_t height;
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
  Vector pos_delta;
} GameObject;

typedef enum { UI_POS_SCREEN, UI_POS_ATTACHED } UIPositionMode;

typedef struct {
  // Position mode: either screen position or attached to object with offset
  UIPositionMode mode;
  // UI Element position: either screen position or attached object with offset
  union {
    Vector screen;

    struct {
      GameObject *object;
      Vector offset;
    } attached;
  } position;

  Sprite *sprite; // UI Element sprite
  int z_index;    // layer index for rendering order
} UIElement;

// Batch of objects and UI elements to render.
typedef struct {
  GameObject **objs;
  uint32_t obj_count;

  UIElement **uis;
  uint32_t ui_count;
} RenderBatch;

// Load sprite from file and scale it.
Sprite load_sprite(const char *path, float scale);
void free_sprite(Sprite *sprite);

// Load spritesheet and split into individual frames.
// Returns array of Sprite (frame_count elements).
//
// Frames are read left-to-right, top-to-bottom.
// Scale is applied to each frame.
Sprite *
load_spritesheet_frames(const char *path, int frame_width, int frame_height, int frame_count, float scale);

// Create sprite from rendered text using given font and color.
Sprite text_sprite(const char *text, TTF_Font *font, SDL_Color color);

void free_sprites(Sprite *frames, int frame_count);

typedef struct {
  Vector pos;
  float w, h;
} Rect;

bool is_rect_intersect(Rect r1, Rect r2);

#endif
