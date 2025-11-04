#include "stb_image.h"
#include <engine/types.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

Sprite load_sprite(const char *path, float scale) {
  Sprite sprite = {0};

  if (scale <= 0.0f) scale = 1.0f;

  int width, height, channels;
  unsigned char *data = stbi_load(path, &width, &height, &channels, 4);
  if (!data) { return sprite; }

  int scaled_width = (int)fmaxf(1.0f, roundf(width * scale));
  int scaled_height = (int)fmaxf(1.0f, roundf(height * scale));

  sprite.width = scaled_width;
  sprite.height = scaled_height;

  sprite.pixels = (uint32_t *)calloc(scaled_width * scaled_height, sizeof(uint32_t));
  if (!sprite.pixels) {
    stbi_image_free(data);
    sprite.width = sprite.height = 0;
    return sprite;
  }

  // Nearest neighbor
  float inv_scale = 1.0f / scale;

  for (int y = 0; y < scaled_height; ++y) {
    int src_y = (int)(y * inv_scale);
    if (src_y >= height) src_y = height - 1;

    for (int x = 0; x < scaled_width; ++x) {
      int src_x = (int)(x * inv_scale);
      if (src_x >= width) src_x = width - 1;

      size_t src_idx = ((size_t)src_y * width + src_x) * 4;

      uint8_t r = data[src_idx + 0];
      uint8_t g = data[src_idx + 1];
      uint8_t b = data[src_idx + 2];
      uint8_t a = data[src_idx + 3];

      sprite.pixels[y * scaled_width + x] =
          ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
    }
  }

  stbi_image_free(data);
  return sprite;
}

void free_sprite(Sprite *sprite) {
  if (!sprite) return;
  if (sprite->pixels) { free(sprite->pixels); }
}
