#include "graphics/render.h"
#include "external/stb_image.h"
#include <stdlib.h>

Sprite load_sprite(const char *path) {
  Sprite sprite = {0};

  int width, height, channels;
  unsigned char *data = stbi_load(path, &width, &height, &channels, 4);

  if (!data) { return sprite; }

  sprite.width = width;
  sprite.height = height;
  sprite.pixels = (uint32_t *)malloc(width * height * sizeof(uint32_t));

  if (!sprite.pixels) {
    stbi_image_free(data);
    sprite.width = 0;
    sprite.height = 0;
    return sprite;
  }

  // Convert RGBA to 0xAARRGGBB (SDL_PIXELFORMAT_ARGB8888)
  for (int i = 0; i < width * height; i++) {
    unsigned char r = data[i * 4 + 0];
    unsigned char g = data[i * 4 + 1];
    unsigned char b = data[i * 4 + 2];
    unsigned char a = data[i * 4 + 3];
    sprite.pixels[i] = (a << 24) | (r << 16) | (g << 8) | b;
  }

  stbi_image_free(data);
  return sprite;
}

// TODO: change scale to float and multiply dimensions instead of division
Sprite load_sprite_scaled(const char *path, int scale) {
  Sprite sprite = {0};

  int width, height, channels;
  unsigned char *data = stbi_load(path, &width, &height, &channels, 4);

  if (!data) { return sprite; }

  // Scaled dimensions
  int scaled_width = width / scale;
  int scaled_height = height / scale;

  if (scaled_width == 0) scaled_width = 1;
  if (scaled_height == 0) scaled_height = 1;

  sprite.width = scaled_width;
  sprite.height = scaled_height;
  sprite.pixels = (uint32_t *)malloc(scaled_width * scaled_height * sizeof(uint32_t));

  if (!sprite.pixels) {
    stbi_image_free(data);
    sprite.width = 0;
    sprite.height = 0;
    return sprite;
  }

  // Nearest neighbor downscaling
  for (int y = 0; y < scaled_height; y++) {
    for (int x = 0; x < scaled_width; x++) {
      int src_x = x * scale;
      int src_y = y * scale;
      int src_idx = (src_y * width + src_x) * 4;

      unsigned char r = data[src_idx + 0];
      unsigned char g = data[src_idx + 1];
      unsigned char b = data[src_idx + 2];
      unsigned char a = data[src_idx + 3];

      sprite.pixels[y * scaled_width + x] = (a << 24) | (r << 16) | (g << 8) | b;
    }
  }

  stbi_image_free(data);
  return sprite;
}

void free_sprite(Sprite *sprite) {
  if (!sprite) return;
  if (sprite->pixels) {
    free(sprite->pixels);
    sprite->pixels = NULL;
  }
  sprite->width = 0;
  sprite->height = 0;
}

void draw_sprite(uint32_t *framebuffer, int fb_width, int fb_height, Sprite *sprite, int x, int y) {
  if (!framebuffer || !sprite || !sprite->pixels) return;

  for (int sy = 0; sy < sprite->height; sy++) {
    for (int sx = 0; sx < sprite->width; sx++) {
      int screen_x = x + sx;
      int screen_y = y + sy;

      if (screen_x < 0 || screen_x >= fb_width || screen_y < 0 || screen_y >= fb_height) {
        continue;
      }

      uint32_t src = sprite->pixels[sy * sprite->width + sx];
      uint8_t src_a = (src >> 24) & 0xFF;
      if (src_a == 0) continue;

      int fb_idx = screen_y * fb_width + screen_x;

      if (src_a == 255) {
        framebuffer[fb_idx] = src;
      } else {
        // Alpha blending
        uint8_t src_r = (src >> 16) & 0xFF;
        uint8_t src_g = (src >> 8) & 0xFF;
        uint8_t src_b = src & 0xFF;

        uint32_t dst = framebuffer[fb_idx];
        uint8_t dst_r = (dst >> 16) & 0xFF;
        uint8_t dst_g = (dst >> 8) & 0xFF;
        uint8_t dst_b = dst & 0xFF;

        uint8_t out_r = (src_r * src_a + dst_r * (255 - src_a)) / 255;
        uint8_t out_g = (src_g * src_a + dst_g * (255 - src_a)) / 255;
        uint8_t out_b = (src_b * src_a + dst_b * (255 - src_a)) / 255;

        framebuffer[fb_idx] = 0xFF000000 | (out_r << 16) | (out_g << 8) | out_b;
      }
    }
  }
}
