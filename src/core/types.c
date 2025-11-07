#include "stb_image.h"
#include <engine/types.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

// Load and scale a rectangular region from image data
// Returns a Sprite with the scaled region
static Sprite load_and_scale_region(const unsigned char *data,
    int image_width,
    int image_height,
    int region_x,
    int region_y,
    int region_width,
    int region_height,
    float scale) {
  Sprite sprite = {0};

  if (!data || scale <= 0.0f) return sprite;
  if (region_x < 0 || region_y < 0) return sprite;
  if (region_x + region_width > image_width) return sprite;
  if (region_y + region_height > image_height) return sprite;

  int scaled_width = (int)fmaxf(1.0f, roundf(region_width * scale));
  int scaled_height = (int)fmaxf(1.0f, roundf(region_height * scale));

  sprite.width = scaled_width;
  sprite.height = scaled_height;
  sprite.pixels = (uint32_t *)calloc(scaled_width * scaled_height, sizeof(uint32_t));

  if (!sprite.pixels) {
    sprite.width = sprite.height = 0;
    return sprite;
  }

  float inv_scale = 1.0f / scale;

  for (int y = 0; y < scaled_height; ++y) {
    int src_y = region_y + (int)(y * inv_scale);
    if (src_y >= region_y + region_height) src_y = region_y + region_height - 1;

    for (int x = 0; x < scaled_width; ++x) {
      int src_x = region_x + (int)(x * inv_scale);
      if (src_x >= region_x + region_width) src_x = region_x + region_width - 1;

      size_t src_idx = ((size_t)src_y * image_width + src_x) * 4;

      uint8_t r = data[src_idx + 0];
      uint8_t g = data[src_idx + 1];
      uint8_t b = data[src_idx + 2];
      uint8_t a = data[src_idx + 3];

      sprite.pixels[y * scaled_width + x] =
          ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
    }
  }

  return sprite;
}

// Load sprite from file and scale it
Sprite load_sprite(const char *path, float scale) {
  Sprite sprite = {0};

  if (scale <= 0.0f) scale = 1.0f;

  int width, height, channels;
  unsigned char *data = stbi_load(path, &width, &height, &channels, 4);
  if (!data) return sprite;

  // Use shared function to load entire image as a region
  sprite = load_and_scale_region(data, width, height, 0, 0, width, height, scale);

  stbi_image_free(data);
  return sprite;
}

void free_sprite(Sprite *sprite) {
  if (!sprite) return;
  if (sprite->pixels) { free(sprite->pixels); }
}

// Load spritesheet and split into individual frames (sprites)
Sprite *load_spritesheet_frames(const char *path,
    int frame_width,
    int frame_height,
    int frame_count,
    float scale) {
  if (!path || frame_width <= 0 || frame_height <= 0 || frame_count <= 0) return NULL;
  if (scale <= 0.0f) scale = 1.0f;

  // Load entire spritesheet
  int width, height, channels;
  unsigned char *data = stbi_load(path, &width, &height, &channels, 4);
  if (!data) return NULL;

  // Calculate grid dimensions
  int cols = width / frame_width;
  int rows = height / frame_height;
  int max_frames = cols * rows;

  // Limit to available frames
  if (frame_count > max_frames) frame_count = max_frames;

  // Allocate array of sprites
  Sprite *frames = (Sprite *)calloc(frame_count, sizeof(Sprite));
  if (!frames) {
    stbi_image_free(data);
    return NULL;
  }

  // Extract each frame (left-to-right, top-to-bottom)
  for (int i = 0; i < frame_count; i++) {
    int col = i % cols;
    int row = i / cols;

    int region_x = col * frame_width;
    int region_y = row * frame_height;

    frames[i] = load_and_scale_region(data,
        width,
        height,
        region_x,
        region_y,
        frame_width,
        frame_height,
        scale);

    // If allocation failed, clean up and return NULL
    if (!frames[i].pixels && (frame_width > 0 && frame_height > 0)) {
      for (int j = 0; j < i; j++) { free_sprite(&frames[j]); }
      free(frames);
      stbi_image_free(data);
      return NULL;
    }
  }

  stbi_image_free(data);
  return frames;
}

void free_spritesheet_frames(Sprite *frames, int frame_count) {
  if (!frames) return;

  for (int i = 0; i < frame_count; i++) { free_sprite(&frames[i]); }

  free(frames);
}

bool is_rect_intersect(Rect r1, Rect r2) {
  return !(r1.pos.x > r2.pos.x + r2.w || r1.pos.x + r1.w < r2.pos.x || r1.pos.y > r2.pos.y + r2.h ||
      r1.pos.y + r1.h < r2.pos.y);
}
