#include "graphics/render.h"
#include <graphics/alpha_blend.h>
#include <stdlib.h>

void draw_sprite(uint32_t *framebuffer,
    int fb_width,
    int fb_height,
    Sprite *sprite,
    int x,
    int y,
    bool flip_horizontal) {
  if (!framebuffer || !sprite || !sprite->pixels) return;

  for (int sy = 0; sy < sprite->height; sy++) {
    for (int sx = 0; sx < sprite->width; sx++) {
      int src_x = flip_horizontal ? (sprite->width - 1 - sx) : sx;

      int screen_x = x + sx;
      int screen_y = y + sy;

      if (screen_x < 0 || screen_x >= fb_width || screen_y < 0 || screen_y >= fb_height) {
        continue;
      }

      uint32_t src = sprite->pixels[sy * sprite->width + src_x];
      int fb_idx = screen_y * fb_width + screen_x;
      uint32_t dst = framebuffer[fb_idx];
      framebuffer[fb_idx] = alpha_blend(src, dst);
    }
  }
}
